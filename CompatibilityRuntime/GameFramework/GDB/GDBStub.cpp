#include <GDB/GDBStub.h>
#include "FileDescriptor.h"
#include <GDB/GDBPacketParser.h>
#include <GDB/GDBPacketFormatter.h>
#include <GDB/DebugMemoryAccess.h>
#include "support.h"
#include <ELF/Image.h>
#include "GlobalContext.h"

#include <cstring>
#include <system_error>
#include <charconv>
#include <vector>
#include <sstream>
#include <signal.h>

#include <dynarmic/interface/A64/a64.h>

GDBStub::GDBStub(const char *configuration, Dynarmic::A64::Jit &jit) : m_packetLayer(configuration), m_signal(SIGTRAP),
    m_jit(jit), m_stopped(true) {

}

GDBStub::~GDBStub() = default;

void GDBStub::beforeEnteringJIT() {

    do {

        if(m_stopped) {
            printf("GDBStub: beforeEnteringJIT on a stopped target, processing commands\n");
            bool resume;
            do {
                auto packet = m_packetLayer.getPacket();

                resume = processPacket(packet);
                if(resume) {
                    printf("GDBStub: resuming the JIT execuion\n");

                    m_stopped = false;
                }
            } while(!resume);
        } else {
            if(m_packetLayer.getInterrupt()) {
                printf("GDBStub: interrupt received, stopping the target\n");
                stopped(SIGINT);
            }
        }

    } while(m_stopped);
}

void GDBStub::stepped() {
    if(m_singleSteppingCycles.has_value()) {
        if(*m_singleSteppingCycles != 0)
            *m_singleSteppingCycles -= 1;

        if(*m_singleSteppingCycles == 0) {
            stopped(SIGTRAP);
        }
    }
}

void GDBStub::stopped(unsigned int signal) {
    printf("GDBStub: stopping on signal %u\n", signal);

    if(m_stopped) {
        throw std::runtime_error("GDBStub::stopped: the target is already stopped");
    }

    m_signal = signal;
    m_stopped = true;
    m_singleSteppingCycles.reset();

    stopResponse();
}


void GDBStub::stopResponse() {
    std::stringstream stopReply;
    stopReply << "S" << std::hex;
    stopReply.width(2);
    stopReply.fill('0');
    stopReply << m_signal;

    m_packetLayer.sendPacket(stopReply.str());
}

bool GDBStub::processPacket(const std::string &packet) {
    printf("Processing a GDB packet: '%s'\n", packet.c_str());

    GDBPacketParser parser(packet);
    if(parser.atEnd()) {
        printf("Empty GDB packet received");
        m_packetLayer.sendPacket();
        return false;
    }

    switch(parser.getCharacter()) {
        case '?':
            stopResponse();
            break;

        case 'g':
        {
            /*
             * Read all registers. The register order is:
             * - all GPRs
             * - SP,
             * - PC,
             * - PSTATE,
             * - all vector registers,
             * - FPSR,
             * - FPCR
             */
            std::vector<unsigned char> packetData;
            packetData.reserve((31 + 2) * 8 + 4 + 32 * 16);
            auto gprs = m_jit.GetRegisters();
            packetData.insert(packetData.end(),
                              reinterpret_cast<unsigned char *>(gprs.data()),
                              reinterpret_cast<unsigned char *>(gprs.data()) + sizeof(gprs));

            uint64_t sp = m_jit.GetSP();
            packetData.insert(packetData.end(),
                              reinterpret_cast<unsigned char *>(&sp),
                              reinterpret_cast<unsigned char *>(&sp) + sizeof(sp));


            uint64_t pc = m_jit.GetPC();
            packetData.insert(packetData.end(),
                              reinterpret_cast<unsigned char *>(&pc),
                              reinterpret_cast<unsigned char *>(&pc) + sizeof(pc));

            uint32_t pstate = m_jit.GetPstate();
            packetData.insert(packetData.end(),
                              reinterpret_cast<unsigned char *>(&pstate),
                              reinterpret_cast<unsigned char *>(&pstate) + sizeof(pstate));

            auto vectors = m_jit.GetVectors();
            packetData.insert(packetData.end(),
                              reinterpret_cast<unsigned char *>(&vectors),
                              reinterpret_cast<unsigned char *>(&vectors) + sizeof(vectors));

            auto fpsr = m_jit.GetFpsr();
            packetData.insert(packetData.end(),
                              reinterpret_cast<unsigned char *>(&fpsr),
                              reinterpret_cast<unsigned char *>(&fpsr) + sizeof(fpsr));

            auto fpcr = m_jit.GetFpcr();
            packetData.insert(packetData.end(),
                              reinterpret_cast<unsigned char *>(&fpcr),
                              reinterpret_cast<unsigned char *>(&fpcr) + sizeof(fpcr));

            GDBPacketFormatter formatter;
            formatter.hex(packetData.data(), packetData.size());

            m_packetLayer.sendPacket(formatter.string());

            break;
        }

        case 'q':
        {
            auto query = parser.getColonDelimited();

            if(query == "Supported") {
                std::stringstream supported;
                supported << "qSupported:PacketSize=" << std::hex << m_packetLayer.packetBufferSize() << ";QStartNoAckMode+;qXfer:features:read+;qXfer:libraries:read+;qXfer:exec-file:read+";

                m_packetLayer.sendPacket(supported.str());

            } else if(query == "Xfer") {
                auto object = parser.getColonDelimited();
                auto command = parser.getColonDelimited();

                if(command == "read") {
                    TransferReadFunction reader;

                    if(object == "features") {
                        reader = &GDBStub::readTargetDescription;

                    } else if(object == "libraries") {
                        reader = &GDBStub::readLibraries;

                    } else if(object == "exec-file") {
                        reader = &GDBStub::readExecFile;

                    } else {
                        // Wrong *object* should give an empty response.
                        printf("Unknown GDB Xfer object on read: '%s'\n", packet.c_str());
                        m_packetLayer.sendPacket();
                        return false;
                    }

                    auto annex = parser.getColonDelimited();

                    auto offset = GDBPacketParser::parseInteger<size_t>(parser.getCommaDelimited());
                    auto length = GDBPacketParser::parseInteger<size_t>(parser.getRest());

                    auto data = (this->*reader)(annex);
                    if(!data.has_value()) {
                        m_packetLayer.sendPacket("E00");
                        return false;
                    }


                    size_t bytesToRead;
                    if(offset > data->size())
                        bytesToRead = 0;
                    else
                        bytesToRead = std::min<size_t>(length, data->size() - offset);

                    std::string response;
                    response.reserve(1 + bytesToRead);
                    if(offset + bytesToRead >= data->size())
                        response.push_back('l');
                    else
                        response.push_back('m');

                    response.append(data->substr(offset, bytesToRead));
                    m_packetLayer.sendPacket(response);

                } else {
                    printf("Unknown GDB Xfer command: '%s'\n", packet.c_str());
                    m_packetLayer.sendPacket();
                }
            } else if(query == "Offsets") {
                std::stringstream offsets;
                offsets << "TextSeg=";
                offsets << std::hex << GlobalContext::get().linkingSet().mainImage()->displacement();
                m_packetLayer.sendPacket(offsets.str());

            } else {
                printf("Unknown GDB query: '%s'\n", packet.c_str());
                m_packetLayer.sendPacket();
            }

            break;
        }

        case 'Q':
        {
            auto set = parser.getColonDelimited();

            if(set == "StartNoAckMode") {
                m_packetLayer.sendPacket("OK");
                m_packetLayer.stopDoingAcks();
            } else {
                printf("Unknown GDB set: '%s'\n", packet.c_str());
                m_packetLayer.sendPacket();
            }
            break;
        }

        case 'm':
        {
            auto address = GDBPacketParser::parseInteger<uintptr_t>(parser.getCommaDelimited());
            auto length = GDBPacketParser::parseInteger<size_t>(parser.getRest());

            std::vector<unsigned char> buf(length);
            auto result = debugMemoryRead(address, buf.data(), length);

            GDBPacketFormatter formatter;
            if(!result)
                formatter.error(EFAULT);
            else
                formatter.hex(buf.data(), buf.size());

            m_packetLayer.sendPacket(formatter.string());

            break;
        }

        case 'M':
        {
            auto address = GDBPacketParser::parseInteger<uintptr_t>(parser.getCommaDelimited());
            auto length = GDBPacketParser::parseInteger<size_t>(parser.getColonDelimited());

            auto bytes = GDBPacketParser::parseHex(parser.getRest());

            GDBPacketFormatter formatter;

            if(bytes.size() != length) {
                formatter.error(EINVAL);
            } else {

                auto result = debugMemoryWrite(address, bytes.data(), bytes.size());

                if(!result)
                    formatter.error(EFAULT);
                else
                    formatter.text("OK");
            }

            m_packetLayer.sendPacket(formatter.string());
            break;
        }

        case 'c':
        {
            if(!parser.atEnd()) {
                m_jit.SetPC(GDBPacketParser::parseInteger<uint64_t>(parser.getRest()));
            }

            return true;
        }

        case 'i':
        {
            if(!parser.atEnd()) {
                m_jit.SetPC(GDBPacketParser::parseInteger<uint64_t>(parser.getCommaDelimited()));
            }

            if(parser.atEnd()) {
                m_singleSteppingCycles.emplace(1);
            } else {
                m_singleSteppingCycles.emplace(GDBPacketParser::parseInteger<size_t>(parser.getRest()));
            }

            return true;
        }

        case 's':
        {
            if(!parser.atEnd()) {
                m_jit.SetPC(GDBPacketParser::parseInteger<uint64_t>(parser.getCommaDelimited()));
            }

            m_singleSteppingCycles.emplace(1);
            return true;
        }

        case 'k':
            m_packetLayer.sendPacket();
            panic("The debugging session has been killed by the debugger.\n");
            break;

        default:
            printf("Unknown GDB packet received: '%s'\n", packet.c_str());
            m_packetLayer.sendPacket();
            break;
    }

    return false;
}

std::optional<std::string_view> GDBStub::readTargetDescription(const std::string_view &annex) {
    static const char *targetDescriptionXML = R"XML(<?xml version="1.0"?>
<!DOCTYPE target SYSTEM "gdb-target.dtd">
<target version="1.0">
    <architecture>aarch64</architecture>
    <feature name="org.gnu.gdb.aarch64.core" />
    <feature name="org.gnu.gdb.aarch64.fpu" />
</target>
)XML";

    if(annex == "target.xml")
        return targetDescriptionXML;
    else
        return {};
}

std::optional<std::string_view> GDBStub::readLibraries(const std::string_view &annex) {
    if(!annex.empty())
        return {};

    if(!m_libraries.has_value()) {

        std::stringstream libraryDesc;
        libraryDesc <<
            "<library-list>\n";

        for(const auto &image: GlobalContext::get().linkingSet().images()) {

        libraryDesc <<
            "<library name=\"" << image->path() << "\" />\n"
                "<segment address=\">0x" << std::hex << image->displacement() << "\" />\n"
            "</library>\n";
        }

        libraryDesc << "</library-list>\n";
        m_libraries = std::move(libraryDesc.str());
    }

    return *m_libraries;
}

std::optional<std::string_view> GDBStub::readExecFile(const std::string_view &annex) {
    if(!annex.empty())
        return {};

    if(!m_execFilename.has_value()) {

        m_execFilename = GlobalContext::get().linkingSet().mainImage()->path();
    }

    return *m_execFilename;
}

#include <cstdio>
#include <cstring>

#include <fstream>
#include <filesystem>
#include <vector>
#include <cinttypes>

#include <ELFPETypes.h>
#include <musl-elf.h>
#include <PETypes.h>


extern const unsigned char ELFPEDOSStub[128];

static constexpr size_t ELFPETargetPageSize = 4096;
static constexpr uint64_t ELFPEImageBase = 0x180000000;

struct BasicELFPEImageHeaders {
    union {
        unsigned char dosStub[sizeof(ELFPEDOSStub)];
        IMAGE_DOS_HEADER dosHeader;
        static_assert(sizeof(dosStub) >= sizeof(dosHeader), "the DOS stub must fit at least the header");
    };
    ELFPEHeader elfPEHeader;
    IMAGE_NT_HEADERS64 ntHeaders;
};

const unsigned char ELFPEDOSStub[128] = {
    0x4d, 0x5a, 0x90, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
    0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x0e, 0x1f, 0xba, 0x0e, 0x00, 0xb4, 0x09, 0xcd, 0x21, 0xb8, 0x01, 0x4c, 0xcd, 0x21, 0x54, 0x68,
    0x69, 0x73, 0x20, 0x70, 0x72, 0x6f, 0x67, 0x72, 0x61, 0x6d, 0x20, 0x63, 0x61, 0x6e, 0x6e, 0x6f,
    0x74, 0x20, 0x62, 0x65, 0x20, 0x72, 0x75, 0x6e, 0x20, 0x69, 0x6e, 0x20, 0x44, 0x4f, 0x53, 0x20,
    0x6d, 0x6f, 0x64, 0x65, 0x2e, 0x0d, 0x0d, 0x0a, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

IMAGE_SECTION_HEADER &appendAndName(std::vector<IMAGE_SECTION_HEADER> &sectionHeaders) {
    std::stringstream sectionName;
    sectionName << "seg" << sectionHeaders.size();

    auto &section = sectionHeaders.emplace_back();
    strncpy(reinterpret_cast<char *>(section.Name), sectionName.str().c_str(), sizeof(section.Name));

    return section;
}

void convertELFToELFPE(const std::filesystem::path &inputFileName, const std::filesystem::path &outputFileName) {
    BasicELFPEImageHeaders headers;

    memset(&headers, 0, sizeof(headers));

    memcpy(&headers.dosStub, ELFPEDOSStub, sizeof(ELFPEDOSStub));

    auto elfPEOffset = offsetof(BasicELFPEImageHeaders, elfPEHeader);
    headers.dosHeader.e_cp = (elfPEOffset + 511) / 512;
    headers.dosHeader.e_cblp = elfPEOffset % 512;
    headers.dosHeader.e_lfanew = offsetof(BasicELFPEImageHeaders, ntHeaders);

    headers.elfPEHeader.signature = ELFPEHeader::ELFPESignature;

    headers.ntHeaders.Signature = 0x00004550;
    headers.ntHeaders.FileHeader.Machine = 0x8664;
    headers.ntHeaders.FileHeader.SizeOfOptionalHeader = sizeof(headers.ntHeaders.OptionalHeader);
    /*
     * Image, Large address aware, debug stripped, DLL
     */
    headers.ntHeaders.FileHeader.Characteristics = 0x2222;

    headers.ntHeaders.OptionalHeader.Magic = 0x20B; // 64-bit PE
    headers.ntHeaders.OptionalHeader.ImageBase = ELFPEImageBase;
    headers.ntHeaders.OptionalHeader.FileAlignment = ELFPETargetPageSize;
    headers.ntHeaders.OptionalHeader.SectionAlignment = ELFPETargetPageSize;
    headers.ntHeaders.OptionalHeader.MajorSubsystemVersion = 4;
    headers.ntHeaders.OptionalHeader.MinorSubsystemVersion = 0;
    headers.ntHeaders.OptionalHeader.Subsystem = 2; // Windows GUI
    // High entropy compatible, dynamic base, NX compatible, do not isolate,
    // no SEH, do not bind, terminal server aware
    headers.ntHeaders.OptionalHeader.DllCharacteristics = 0x8f60;
    headers.ntHeaders.OptionalHeader.NumberOfRvaAndSizes =
        sizeof(headers.ntHeaders.OptionalHeader.DataDirectory) / sizeof(headers.ntHeaders.OptionalHeader.DataDirectory[0]);

    std::ifstream elfStream;
    elfStream.exceptions(std::ios::failbit | std::ios::eofbit | std::ios::badbit);
    elfStream.open(inputFileName, std::ios::in | std::ios::binary);

    Elf64_Ehdr ehdr;
    elfStream.read(reinterpret_cast<char *>(&ehdr), sizeof(ehdr));
    static const unsigned char elfMagic[EI_ABIVERSION] = {
        ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3,
        ELFCLASS64, ELFDATA2LSB, EV_CURRENT
    };

    if(memcmp(ehdr.e_ident, elfMagic, sizeof(elfMagic)) != 0 || ehdr.e_version != EV_CURRENT ||
        ehdr.e_phentsize != sizeof(Elf64_Phdr) || ehdr.e_phnum == 0)
        throw std::runtime_error("bad ELF identification");

    elfStream.seekg(ehdr.e_phoff);

    std::vector<Elf64_Phdr> phdrs(ehdr.e_phnum);
    elfStream.read(reinterpret_cast<char *>(phdrs.data()), phdrs.size() * sizeof(Elf64_Phdr));

    std::vector<IMAGE_SECTION_HEADER> sectionHeaders;
    size_t sizeOfImageWithoutHeaders = 0;
    size_t sizeOfImageDataWithoutHeaders = 0;

    /*
     * Emit sections for program segments.
     */
    for(const auto &phdr: phdrs) {
        if(phdr.p_type == PT_LOAD) {
            if((phdr.p_offset & (ELFPETargetPageSize - 1)) != (phdr.p_vaddr & (ELFPETargetPageSize - 1)))
                throw std::runtime_error("ELF-PE target page size is greater than the maximum page size specified during the ELF link");

            printf("PT_LOAD: flags 0x%08" PRIx32 " vaddr 0x%08" PRIx64 " offset 0x%08" PRIx64 " filesz 0x%08" PRIx64 " memsz 0x%08" PRIx64 "\n",
                   phdr.p_flags, phdr.p_vaddr, phdr.p_offset, phdr.p_filesz, phdr.p_memsz);

            DWORD thisSectionCharacteristics;

            if(phdr.p_filesz == 0) {
                thisSectionCharacteristics = 0x80; // IMAGE_SCN_CNT_UNINITIALIZED_DATA
            } else {
                thisSectionCharacteristics = 0x40; // IMAGE_SCN_CNT_INITIALIZED_DATA
            }

            if(phdr.p_flags & PF_R)
                thisSectionCharacteristics |= 0x40000000; // IMAGE_SCN_MEM_READ

            if(phdr.p_flags & PF_W)
                thisSectionCharacteristics |= UINT32_C(0x80000000); // IMAGE_SCN_MEM_READ

            DWORD overlapSectionCharacteristics = thisSectionCharacteristics;

            auto endOfSectionMemoryData = phdr.p_vaddr + phdr.p_memsz;
            auto endOfSectionFileData = phdr.p_vaddr + phdr.p_filesz;

            auto overlapSectionStartVAddr = phdr.p_vaddr & ~(ELFPETargetPageSize - 1);
            auto overlapSectionStartOffset = phdr.p_offset & ~(ELFPETargetPageSize - 1);

            auto mainSectionStartVAddr = (phdr.p_vaddr + (ELFPETargetPageSize - 1)) & ~(ELFPETargetPageSize - 1);
            auto mainSectionStartOffset = (phdr.p_offset + (ELFPETargetPageSize - 1)) & ~(ELFPETargetPageSize - 1);

            if(sectionHeaders.empty()) {
                if(overlapSectionStartVAddr != 0) {
                    throw std::runtime_error("the first section doesn't start at the address 0");
                }
            } else {
                const auto &last = sectionHeaders.back();

                size_t endOfLast = (last.VirtualAddress + last.Misc.VirtualSize + (ELFPETargetPageSize - 1)) & ~(ELFPETargetPageSize - 1);
                if(endOfLast != overlapSectionStartVAddr) {
                    if(endOfLast > overlapSectionStartVAddr)
                        throw std::runtime_error("overlapping sections");
                    else {
                        printf("filling a gap between sections: 0x%08zX - 0x%08zX\n", endOfLast, overlapSectionStartVAddr);
                        auto &pad = appendAndName(sectionHeaders);
                        pad.VirtualAddress = endOfLast;
                        pad.Misc.VirtualSize = overlapSectionStartVAddr - endOfLast;
                        pad.Characteristics = 0x80; // IMAGE_SCN_CNT_UNINITIALIZED_DATA
                    }
                }
            }

            if(overlapSectionStartVAddr != mainSectionStartVAddr) {
                if(!sectionHeaders.empty()) {
                    auto &lastSection = sectionHeaders.back();
                    auto endOfLastSectionVirtual = lastSection.VirtualAddress + lastSection.Misc.VirtualSize;

                    endOfLastSectionVirtual = std::min<size_t>(endOfLastSectionVirtual, overlapSectionStartVAddr);
                    lastSection.Misc.VirtualSize = endOfLastSectionVirtual - lastSection.VirtualAddress;


                    if(lastSection.SizeOfRawData != 0) {
                        auto endOfLastSectionFile = lastSection.VirtualAddress + lastSection.SizeOfRawData;

                        endOfLastSectionFile = std::min<size_t>(endOfLastSectionFile, overlapSectionStartVAddr);
                        lastSection.SizeOfRawData = endOfLastSectionFile - lastSection.VirtualAddress;
                    }

                    overlapSectionCharacteristics |= lastSection.Characteristics;
                }
            }

            if(overlapSectionCharacteristics == thisSectionCharacteristics) {
                /*
                 * If the characteristics are the same, then we can coalesce the sections.
                 */
                mainSectionStartOffset = overlapSectionStartOffset;
                mainSectionStartVAddr = overlapSectionStartVAddr;
            }

            if(overlapSectionStartVAddr != mainSectionStartVAddr) {
                auto &overlapSection = appendAndName(sectionHeaders);
                overlapSection.VirtualAddress = overlapSectionStartVAddr;
                if(endOfSectionFileData > overlapSectionStartVAddr) {
                    overlapSection.PointerToRawData = overlapSectionStartOffset;

                    auto rawDataLimit = std::min<size_t>(endOfSectionFileData, mainSectionStartVAddr);

                    overlapSection.SizeOfRawData =
                        ((rawDataLimit - overlapSection.VirtualAddress) + (ELFPETargetPageSize - 1))
                        & ~(ELFPETargetPageSize - 1);
                }

                overlapSection.Misc.VirtualSize = overlapSectionStartVAddr - mainSectionStartVAddr;
                overlapSection.Characteristics = overlapSectionCharacteristics;

            }

            if(endOfSectionMemoryData > mainSectionStartVAddr) {
                auto &mainSection = appendAndName(sectionHeaders);
                mainSection.VirtualAddress = mainSectionStartVAddr;
                if(endOfSectionFileData > mainSectionStartVAddr) {
                    mainSection.PointerToRawData = mainSectionStartOffset;
                    mainSection.SizeOfRawData =
                        ((endOfSectionFileData - mainSection.VirtualAddress) + (ELFPETargetPageSize - 1))
                        & ~(ELFPETargetPageSize - 1);
                }
                mainSection.Misc.VirtualSize = endOfSectionMemoryData - mainSectionStartVAddr;
                mainSection.Characteristics = thisSectionCharacteristics;
            }

            sizeOfImageWithoutHeaders = std::max<size_t>(sizeOfImageWithoutHeaders,
                (endOfSectionMemoryData + (ELFPETargetPageSize - 1)) & ~(ELFPETargetPageSize - 1));

            if(phdr.p_offset <= ehdr.e_phoff && phdr.p_offset + phdr.p_filesz >=
                ehdr.e_phoff + ehdr.e_phnum * sizeof(Elf64_Phdr)) {

                headers.elfPEHeader.phdrRVA = phdr.p_vaddr + (ehdr.e_phoff - phdr.p_offset);
                headers.elfPEHeader.phdrSize = ehdr.e_phnum * sizeof(Elf64_Phdr);
            }

            if(phdr.p_filesz > 0) {

                sizeOfImageDataWithoutHeaders = std::max<size_t>(sizeOfImageDataWithoutHeaders,
                    (phdr.p_offset + phdr.p_filesz + ELFPETargetPageSize - 1) & ~(ELFPETargetPageSize - 1));
            }
        }
    }

    /*
     * Now that the number of sections is known, we can calculate the header size.
     */
    auto sizeOfHeaders = sizeof(headers) + sectionHeaders.size() * sizeof(IMAGE_SECTION_HEADER);
    sizeOfHeaders = (sizeOfHeaders + ELFPETargetPageSize - 1) & ~(ELFPETargetPageSize - 1);

    headers.elfPEHeader.elfOffset = sizeOfHeaders;
        headers.ntHeaders.FileHeader.NumberOfSections = sectionHeaders.size();
    headers.ntHeaders.OptionalHeader.SizeOfHeaders = sizeOfHeaders;

    /*
     * Now that the final size of headers is known, fix up the section headers to include it.
     */
    for(auto &header:sectionHeaders) {
        header.VirtualAddress += sizeOfHeaders;

        if(header.SizeOfRawData != 0) {
            header.PointerToRawData += sizeOfHeaders;
        }
    }

    if(headers.elfPEHeader.phdrSize != 0)
        headers.elfPEHeader.phdrRVA += sizeOfHeaders;

    auto sizeOfImage = sizeOfHeaders + sizeOfImageWithoutHeaders;

    sizeOfImage = (sizeOfImage + ELFPETargetPageSize - 1) & ~(ELFPETargetPageSize - 1);

    headers.ntHeaders.OptionalHeader.SizeOfImage = sizeOfImage;

    std::ofstream stream;
    stream.exceptions(std::ios::failbit | std::ios::eofbit | std::ios::badbit);
    stream.open(outputFileName, std::ios::out | std::ios::binary | std::ios::trunc);

    stream.write(reinterpret_cast<const char *>(&headers), sizeof(headers));
    stream.write(reinterpret_cast<const char *>(sectionHeaders.data()),
                 sectionHeaders.size() * sizeof(IMAGE_SECTION_HEADER));

    if(stream.tellp() < sizeOfHeaders) {
        stream.seekp(sizeOfHeaders - 1, std::ios::beg);
        char dummy = 0;
        stream.write(&dummy, 1);
    }

    elfStream.exceptions(std::ios::badbit);
    elfStream.seekg(0);

    std::vector<char> chunk(65536);

    size_t bytesRead;
    do {
        elfStream.read(chunk.data(), chunk.size());
        bytesRead = elfStream.gcount();

        stream.write(chunk.data(), bytesRead);
    } while(bytesRead == chunk.size());

    if(stream.tellp() < sizeOfImageDataWithoutHeaders + sizeOfHeaders) {
        stream.seekp(sizeOfImageDataWithoutHeaders + sizeOfHeaders - 1, std::ios::beg);
        char dummy = 0;
        stream.write(&dummy, 1);
    }
}

int main(int argc, char **argv) {
    if(argc != 3) {
        fprintf(stderr, "Usage: %s <INPUT ELF FILE> <OUTPUT ELF-PE FILE>\n", argv[0]);
        return 1;
    }

    convertELFToELFPE(argv[1], argv[2]);

    return 0;
}

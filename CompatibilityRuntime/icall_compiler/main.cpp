#include <translator_api.h>

#include "ArgumentPackage.h"
#include "ICallMethodRegistry.h"

#include <cstdio>
#include <fstream>
#include <zlib.h>

static void writeTypeDecl(std::ostream &generated, const ICallMethod &method, bool interposed) {

    generated << "  typedef " << method.returnType().getCType() << "(*" << method.cName() << "_";

    if(interposed)
        generated << "Interposer";

    generated << "Type)(";

    bool isFirst = true;
    for(const auto &arg: method.parameterTypes()) {
        if(isFirst)
            isFirst = false;
        else
            generated << ", ";

        generated << arg.getCType();
    }

    if(interposed) {
        if(!isFirst)
            generated << ", ";

        generated << method.cName() << "_Type methodImpl";
    }

    generated << ");\n";
}

static void writeMethodImpl(std::ostream &generated, const ICallMethod &method, bool interposed) {

    generated << "  static void thunk_";
    if(interposed)
        generated << "interposed_";

    generated << method.cName() << "(ARMMachineContext &ctx) {\n";

    auto argCount = method.parameterTypes().size();

    if(argCount == 0 && method.returnType().isVoid()) {
        generated << "    (void)ctx;\n";
    }

    if(!method.returnType().isVoid()) {
        generated << "    *reinterpret_cast<" << method.returnType().getCType() << " *>(";

        if(method.returnType().isVector()) {
            generated << "&ctx.vectors[0]";
        } else {
            generated << "&ctx.gprs[0]";
        }

        generated << ") = \n  ";
    }

    generated << "    ";
    if(interposed) {
        generated << "interposer_";
    } else {
        generated << "handler_";
    }

    generated << method.cName() << "(";

    static constexpr unsigned int MaximumRegisterIntegers = 8;
    static constexpr unsigned int MaximumRegisterVectors = 8;

    unsigned int integerSlot = 0;
    unsigned int vectorSlot = 0;
    unsigned int spOffset = 0;

    for(size_t argno = 0; argno < argCount; argno++) {
        if(argno != 0) {
            generated << ",";
        }

        generated << "\n";

        if(!method.returnType().isVoid()) {
            generated << "  ";
        }

        const auto &parameter = method.parameterTypes()[argno];

        generated << "      *reinterpret_cast<" << parameter.getCType() << " const *>(";

        if(parameter.isVector()) {
            if(vectorSlot < MaximumRegisterVectors) {
                generated << "&ctx.vectors[" << vectorSlot << "]";
                vectorSlot++;
            } else {
                throw std::runtime_error("fetching vector args from stack is not implemented yet");
            }
        } else {
            if(integerSlot < MaximumRegisterIntegers) {
                generated << "&ctx.gprs[" << integerSlot << "]";
                integerSlot++;
            } else {
                generated << "ctx.sp + " << spOffset;
                spOffset += 8;
            }
        }

        generated << ")";

    }

    if(interposed) {
        if(argCount != 0) {
            generated << ",";
        }

        generated << "\n";

        if(!method.returnType().isVoid()) {
            generated << "  ";
        }

        generated << "      handler_" << method.cName();
    }

    generated << ");\n";


    generated <<
        "  };\n"
        "\n";
}

static int compilerMain(void *arg) {
    const auto &args = static_cast<const ArgumentPackage *>(arg);

    il2cpp_set_data_dir("../NieR_Data/il2cpp_data");
    auto result = il2cpp_init("IL2CPP Root Domain");
    if(!result) {
        fprintf(stderr, "il2cpp has failed to initialize.\n");
        return 1;
    }

    printf("il2cpp has been initialized\n");

    ICallMethodRegistry registry;

    il2cpp_shutdown();

    std::ofstream generated;
    generated.exceptions(std::ios::failbit | std::ios::eofbit | std::ios::badbit);
    generated.open("compiled_icalls.cpp", std::ios::out | std::ios::trunc | std::ios::binary);

    generated << R"SRC(

#include <cstdint>
#include <cstring>
#include <array>

#include <zlib.h>

#include <ARMMachineContext.h>
#include <PrecompiledICallInterface.h>

namespace {
  struct ICallMethodDeclaration {
    const char *name;
    ICallPrecompiledThunks thunks;
  };

)SRC";

    for(const auto &method: registry.methods()) {
        generated << "  // " << method.name() << "\n";

        writeTypeDecl(generated, method, false);
        writeTypeDecl(generated, method, true);

        generated <<
            "  static " << method.cName() << "_Type handler_" << method.cName() << " = nullptr;\n"
            "  static " << method.cName() << "_InterposerType interposer_" << method.cName() << " = nullptr;\n"
            "\n";

        writeMethodImpl(generated, method, false);
        writeMethodImpl(generated, method, true);

        generated << "\n\n";
    }

    generated << "  static const ICallMethodDeclaration methods[] {\n";

    for(const auto &method: registry.methods()) {
        generated <<
            "    {\n"
            "      .name = \"" << method.name() << "\",\n"
            "      .thunks = {\n"
            "        .withoutInterposer = thunk_" << method.cName() << ",\n"
            "        .withInterposer = thunk_interposed_" << method.cName() << ",\n"
            "        .icallHandler = reinterpret_cast<ICallTypeErasedTarget *>(&handler_" << method.cName() << "),\n"
            "        .icallInterposer = reinterpret_cast<ICallTypeErasedTarget *>(&interposer_" << method.cName() << ")\n"
            "      },\n"
            "    },\n";
    }

    generated << "  };\n";

    printf("%zu icall method thunks generated\n", registry.methods().size());

    std::vector<uint16_t> buckets(997, 0);
    std::vector<uint16_t> chain(registry.methods().size(), 0);

    unsigned int bucketsInUse = 0;
    unsigned int longestChain = 0;

    for(const auto &method: registry.methods()) {
        auto hash = crc32_z(0, reinterpret_cast<const Bytef *>(method.name().data()), method.name().size());

        auto bucketno = hash % buckets.size();

        auto prev = buckets[bucketno];
        unsigned int chainLength = 0;

        if(prev == 0) {
            buckets[bucketno] = 1 + (&method - registry.methods().data());
            bucketsInUse++;
        } else {

            auto chainpos = buckets[bucketno];
            while(true) {
                chainLength += 1;

                auto next = chain[chainpos - 1];

                if(next == 0)
                    break;

                chainpos = next;
            }

            chain[chainpos - 1] = 1 + (&method - registry.methods().data());
        }

        if(chainLength > longestChain)
            longestChain = chainLength;
    }

    printf("%u out of %zu hash buckets are in use (%zu %%), maximum chain length: %u\n", bucketsInUse, buckets.size(), bucketsInUse * 100 / buckets.size(), longestChain);

    generated << "static const uint16_t hashBuckets[]{";

    for(const auto &entry: buckets) {
        if((&entry - buckets.data()) % 16 == 0)
            generated << "\n    ";

        generated << entry << ", ";
    }

    generated << "\n};\n";

    generated << "static const uint16_t hashChain[]{";

    for(const auto &entry: chain) {
        if((&entry - chain.data()) % 16 == 0)
            generated << "\n    ";

        generated << entry << ", ";
    }

    generated << "\n};\n";

    generated << "}";

    generated << R"SRC(

extern "C"
#if defined(_WIN32)
__attribute__((dllexport))
#else
__attribute__((visibility("default")))
#endif
const ICallPrecompiledThunks *get_precompiled_icall_thunks(const char *icallName) {

    auto bucket = crc32_z(0, reinterpret_cast<const Bytef *>(icallName), strlen(icallName)) % (sizeof(hashBuckets) / sizeof(hashBuckets[0]));

    for(auto chain = hashBuckets[bucket]; chain != 0; chain = hashChain[chain - 1]) {
        const auto &entry = methods[chain - 1];
        if(strcmp(entry.name, icallName) == 0) {
            return &entry.thunks;
        }
    }

    return nullptr;
}
)SRC";


    return 0;
}

int main(int argc, char **argv) {
    ArgumentPackage args = {
        .argc = argc,
        .argv = argv
    };

    translator_disallow_precompiled_icall_use();

    return translator_main(compilerMain, &args);
}

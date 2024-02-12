#include "GlobalContext.h"

#include <Translator/GCHooks.h>
#include <Translator/thunking.h>

#include <stdexcept>

#include <sys/random.h>
#include <ELF/musl-elf.h>
#include <dlfcn.h>

#include "SystemAPIThunking.h"

#include <Bionic/BionicABITypes.h>
#include <Bionic/BionicCallouts.h>

const uint32_t GlobalContext::PageSize = queryPageSize();

GlobalContext *GlobalContext::GlobalContextRegisterer::m_context = nullptr;

GlobalContext::GlobalContext() : m_registerer(this) {

    auto directory = thisLibraryDirectory();
    m_armlib.emplace(directory / "bionic.so");
    m_il2cpp.emplace(directory / "libil2cpp.so");

    bindBionicCallouts(*m_armlib);

    installGCHooks(*m_il2cpp);

    // We don't run the dynamic linker (yet?), so we need to do some handover
    // stuff the linker does.

    auto &context = JITThreadContext::get();

    static BionicKernelArgumentBlock kernelArgumentBlock;
    memset(&kernelArgumentBlock, 0, sizeof(kernelArgumentBlock));

    static unsigned char randomBytes[16];
    getrandom(randomBytes, sizeof(randomBytes), 0);

    static const char *argv[] = { "dummy", nullptr };
    static const Elf64_auxv_t auxVector[]{
        { AT_RANDOM, reinterpret_cast<uintptr_t>(randomBytes) },
        { AT_NULL }
    };
    kernelArgumentBlock.argc = 1;
    kernelArgumentBlock.argv = const_cast<char **>(argv);
    kernelArgumentBlock.envp = environ;
    kernelArgumentBlock.auxv = auxVector;


    printf("Running bionic's __compatibility_runtime_init_libc\n");
    bionic_init_libc(&kernelArgumentBlock);

    printf("Running bionic constructors\n");
    m_armlib->runConstructors();

    printf("Running il2cpp constructors\n");
    m_il2cpp->runConstructors();
}

GlobalContext::~GlobalContext() {
    printf("Releasing the main thread context, %p\n", &JITThreadContext::get());

    JITThreadContext::clearCurrentThreadContext();

    printf("Translator's GlobalContext is now waiting for all of the remaining threads to terminate.\n");

    JITThreadContext::waitForAllThreadsToExit();
}

GlobalContext::GlobalContextRegisterer::GlobalContextRegisterer(GlobalContext *context) {
    if(m_context)
        throw std::logic_error("the translator global context has already been created");

    m_context = context;
}

GlobalContext::GlobalContextRegisterer::~GlobalContextRegisterer() {
    m_context = nullptr;
}

void GlobalContext::GlobalContextRegisterer::missingGlobalContext() {
    throw std::logic_error("attempted to get the translator global context when none exists. Make sure that any ARM world APIs are called from within translator_main");
}

std::filesystem::path GlobalContext::thisLibraryDirectory() {
        static unsigned char thisImageSymbol;

    Dl_info info;

    if(!dladdr(&thisImageSymbol, &info))
        throw std::runtime_error("Unable to resolve own library");

    return std::filesystem::absolute(std::filesystem::path(info.dli_fname)).parent_path();
}

uint32_t GlobalContext::queryPageSize() {

    auto result = sysconf(_SC_PAGESIZE);
    if(result < 0) {
        throw std::system_error(errno, std::generic_category());
    }

    return static_cast<uint32_t>(result);
}

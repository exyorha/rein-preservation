#include "GlobalContext.h"

#include <stdexcept>

#include <dlfcn.h>

const uint32_t GlobalContext::PageSize = queryPageSize();

GlobalContext *GlobalContext::GlobalContextRegisterer::m_context = nullptr;

GlobalContext::GlobalContext() : m_registerer(this) {

    auto directory = thisLibraryDirectory();
    m_armlib.emplace(directory / "armlib.so");
    m_il2cpp.emplace(directory / "libil2cpp.so");
}

GlobalContext::~GlobalContext() = default;

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

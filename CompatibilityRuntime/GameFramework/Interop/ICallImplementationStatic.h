#ifndef ICALL_IMPLEMENTATION_STATIC_H
#define ICALL_IMPLEMENTATION_STATIC_H

#include <filesystem>
#include <mutex>
#include <unordered_map>

#include "ICallImplementation.h"

#include <PlatformDynamicLibrary.h>

#include <PrecompiledICallInterface.h>

class ICallImplementationStatic final : public ICallImplementation {
public:
    explicit ICallImplementationStatic(const std::filesystem::path &libraryPath);
    ~ICallImplementationStatic() override;

    void *installICallHandler(const char *name, Il2CppMethodPointer implementation) override;
    Il2CppMethodPointer resolveNativeICall(const char *name, void *handler) override;

private:
    static void thunkCall() noexcept;

    PlatformDynamicLibrary m_thunkLibrary;
    LookupPrecompiledICallThunkFunc m_lookup;
    std::mutex m_unknownICallTableMutex;
    std::unordered_map<std::string_view, Il2CppMethodPointer> m_unknownICallTable;
};

#endif


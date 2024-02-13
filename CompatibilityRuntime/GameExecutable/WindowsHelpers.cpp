#include "WindowsHelpers.h"

#include <vector>
#include <stdexcept>

std::filesystem::path getPathToModule(HMODULE moduleHandle) {

    std::vector<wchar_t> modulePathChars(PATH_MAX);
    DWORD outLength;
    DWORD error;

    do {
        outLength = GetModuleFileName(moduleHandle, modulePathChars.data(), modulePathChars.size());
        if(outLength == 0) {
            throw std::runtime_error("GetModuleFileName failed");
        }

        if(error == ERROR_INSUFFICIENT_BUFFER) {
            modulePathChars.resize(modulePathChars.size() * 2);
        }

    } while(error == ERROR_INSUFFICIENT_BUFFER);

    return modulePathChars.data();
}

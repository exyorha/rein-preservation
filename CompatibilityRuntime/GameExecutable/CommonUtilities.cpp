#include "CommonUtilities.h"

#ifdef _WIN32
#include "WindowsHelpers.h"
#include <shlobj.h>
#endif

bool stringsEqualIgnoreCaseAsciiOnly(const char *a, const char *b) {
    char chB;

    while(*a) {
        char chA = *a++;
        chB = *b++;

        if(chA >= 'A' && chA <= 'Z')
            chA = chA - 'A' + 'a';

        if(chB >= 'A' && chB <= 'Z')
            chB = chB - 'A' + 'a';

        if(chA != chB)
            return false;
    }

    return *b == 0;

}

std::filesystem::path executablePath() {
    std::filesystem::path executablePath;

#ifdef _WIN32
    executablePath = getPathToModule(nullptr);
#else
    executablePath = std::filesystem::read_symlink("/proc/self/exe");
#endif

    return executablePath;
}


std::filesystem::path executableDirectory() {

    return executablePath().parent_path();
}

#if defined(_WIN32)
std::filesystem::path getApplicationDataDir(void) {
    struct StringFreer {
        PWSTR string = nullptr;

        ~StringFreer() {
            if(string)
                CoTaskMemFree(string);
        }
    } path;

    auto result = SHGetKnownFolderPath(FOLDERID_LocalAppDataLow, KF_FLAG_CREATE, nullptr, &path.string);
    if(FAILED(result))
        throw std::runtime_error("SHGetKnownFolderPath has failed");

    return std::filesystem::path(path.string) / "SQUARE ENIX Co_,Ltd_" / "NieR";
}

#endif

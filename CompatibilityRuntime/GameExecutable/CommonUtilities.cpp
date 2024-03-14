#include "CommonUtilities.h"

#ifdef _WIN32
#include "WindowsHelpers.h"
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

std::filesystem::path executableDirectory() {
    std::filesystem::path executablePath;

#ifdef _WIN32
    executablePath = getPathToModule(nullptr);
#else
    executablePath = std::filesystem::read_symlink("/proc/self/exe");
#endif

    return executablePath.parent_path();
}

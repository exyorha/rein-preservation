#include <windows.h>
#include <shellapi.h>

#include <translator_api.h>
#include <unity_stub.h>

#include <stdexcept>
#include <vector>
#include <string_view>

#include "GameEntryPoint.h"
#include "Il2CppUtilities.h"

struct ArgumentsPackageWindows {
    HINSTANCE hInstance;
    HINSTANCE hPrevInstance;
    const wchar_t *lpCommandLine;
    int nCmdShow;
};


static int invokeUnityWindows(void *package) {
    auto args = static_cast<ArgumentsPackageWindows *>(package);

    return UnityMain(args->hInstance, args->hPrevInstance, args->lpCommandLine, args->nCmdShow);
}

static int gameMainWindows(void *arg) {
    auto args = static_cast<ArgumentsPackageWindows *>(arg);

    struct WideARGV {
        LPWSTR *argv = nullptr;

        ~WideARGV() {
            if(argv)
                LocalFree(argv);
        }
    } wideARGV;

    int wideARGC;
    wideARGV.argv = CommandLineToArgvW(args->lpCommandLine, &wideARGC);
    if(wideARGV.argv == nullptr)
        throw std::runtime_error("CommandLineToArgvW has failed");

    std::vector<std::string> arguments;
    arguments.reserve(wideARGC);

    for(int argno = 0; argno < wideARGC; argno++) {
        std::u16string_view argstring(reinterpret_cast<const char16_t *>(wideARGV.argv[argno]));

        arguments.emplace_back(utf16ToUtf8(argstring));
    }

    std::vector<char *> argumentPointers;
    argumentPointers.reserve(arguments.size() + 1);

    for(auto &argument: arguments) {
        argumentPointers.emplace_back(argument.data());
    }

    argumentPointers.emplace_back(nullptr);

    return gameMain(arguments.size(), argumentPointers.data(), invokeUnityWindows, args);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, wchar_t *lpCommandLine, int nShow) {
    ArgumentsPackageWindows args{
        .hInstance = hInstance,
        .hPrevInstance = hPrevInstance,
        .lpCommandLine = lpCommandLine,
        .nCmdShow = nShow
    };


    if(!gameEarlyInit())
        return 1;

    return translator_main(gameMainWindows, &args);
}

#include <unistd.h>
#include <windows.h>
#include <winternl.h>
#include <shellapi.h>
#include <fcntl.h>

#include <translator_api.h>

#include <stdexcept>
#include <vector>
#include <string_view>

#include <GLES/WGL/WindowsImportRedirection.h>

#include "GameEntryPoint.h"
#include "CommonUtilities.h"
#include <Il2CppUtilities.h>

namespace {

    struct ArgumentsPackageWindows {
        HINSTANCE hInstance;
        HINSTANCE hPrevInstance;
        const wchar_t *lpCommandLine;
        int nCmdShow;
    };

    struct WideARGV {
        LPWSTR *argv;

        WideARGV() : argv(nullptr) {}

        WideARGV(const WideARGV &other) = delete;
        WideARGV &operator =(const WideARGV &other) = delete;

        ~WideARGV() {
            if(argv)
                LocalFree(argv);
        }
    };
}

static std::wstring commandLineString;

typedef int (*UnityMainType)(HINSTANCE hInstance, HINSTANCE hPrevInstance, const wchar_t *lpCommandLine, int nShow);

static UnityMainType UnityMainPtr;

static LPWSTR WINAPI replacement_GetCommandLineW(void) {
    return commandLineString.data();
}

static const ReplacementExport replacementKernelFunctions[]{
    { "GetCommandLineW", reinterpret_cast<void *>(replacement_GetCommandLineW), nullptr },
};

static int invokeUnityWindows(void *package) {
    auto args = static_cast<ArgumentsPackageWindows *>(package);

    return UnityMainPtr(args->hInstance, args->hPrevInstance, args->lpCommandLine, args->nCmdShow);
}

static int gameMainWindows(void *arg) {
    auto args = static_cast<ArgumentsPackageWindows *>(arg);

    std::vector<std::string> arguments;

    arguments.emplace_back("GameExecutable");

    if(*args->lpCommandLine) {

        WideARGV wideARGV;
        int wideARGC;
        wideARGV.argv = CommandLineToArgvW(args->lpCommandLine, &wideARGC);
        if(wideARGV.argv == nullptr)
            throw std::runtime_error("CommandLineToArgvW has failed");

        arguments.reserve(arguments.size() + wideARGC);

        for(int argno = 0; argno < wideARGC; argno++) {
            std::u16string_view argstring(reinterpret_cast<const char16_t *>(wideARGV.argv[argno]));

            const auto &arg = arguments.emplace_back(utf16ToUtf8(argstring));
        }
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

    {

        bool noLog = false;
        bool appendLog = false;
        const wchar_t * specifiedLogFile = nullptr;

        WideARGV wideARGV;
        int wideARGC;
        wideARGV.argv = CommandLineToArgvW(lpCommandLine, &wideARGC);
        if(wideARGV.argv == nullptr)
            throw std::runtime_error("CommandLineToArgvW has failed");

        for(int argno = 0; argno < wideARGC; argno++) {
            auto arg = wideARGV.argv[argno];

            if(wcsicmp(arg, L"-nolog") == 0) {
                noLog = true;
            } else if(wcsicmp(arg, L"-logfile") == 0) {
                if(++argno == wideARGC)
                    break;

                specifiedLogFile = wideARGV.argv[argno];
                if(wcscmp(specifiedLogFile, L"-") == 0 || wcscmp(specifiedLogFile, L"") == 0)
                    noLog = true;

            } else if(wcsicmp(arg, L"-appendlog") == 0) {
                appendLog = true;

            } else {
                if(!commandLineString.empty())
                    commandLineString.push_back(L' ');

                commandLineString.push_back(L'"');
                for(const wchar_t *ptr = arg; *ptr; ptr++) {
                    auto ch = *ptr;

                    if(ch == L'\\' || ch == L'"') {
                        commandLineString.push_back(L'\\');
                    }
                    commandLineString.push_back(ch);
                }

                commandLineString.push_back(L'"');;
            }
        }

        if(!commandLineString.empty())
            commandLineString.push_back(L' ');

        commandLineString.append(L"-nolog");

        if(!noLog) {

            std::filesystem::path logfile;

            if(specifiedLogFile) {
                logfile = specifiedLogFile;
            } else {
                logfile = getApplicationDataDir() / "Player.log";
            }

            if(logfile.has_parent_path())
                std::filesystem::create_directories(logfile.parent_path());


            if(!appendLog) {
                std::filesystem::path oldLog(logfile);

                std::string stem = oldLog.stem().string();
                stem.append("-prev");
                stem.append(oldLog.extension().string());
                oldLog.replace_filename(stem);

                std::error_code ec;

                std::filesystem::remove(oldLog, ec);
                std::filesystem::rename(logfile, oldLog, ec);

            }

            auto earlyBoot = CreateFile(logfile.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
                                        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
            if(earlyBoot != INVALID_HANDLE_VALUE) {

                int stdfd = _open_osfhandle((intptr_t)earlyBoot, _O_APPEND);
                close(STDIN_FILENO);
                dup2(stdfd, STDOUT_FILENO);
                dup2(stdfd, STDERR_FILENO);
                close(stdfd);

                fclose(stdout);

                *stdout = *fdopen(STDOUT_FILENO, "wb");

                setvbuf(stdout, nullptr, _IONBF, 256);

                fclose(stderr);

                *stderr = *fdopen(STDERR_FILENO, "wb");

                setvbuf(stderr, nullptr, _IONBF, 256);

                // Make the standard handles available for use even if they're currently used for GUI things
                auto windowFlags =
                    reinterpret_cast<ULONG *>(reinterpret_cast<unsigned char *>(
                        NtCurrentTeb()->ProcessEnvironmentBlock->ProcessParameters) + 0xA4);

                *windowFlags &= ~(0x800 | 0x400);

                SetStdHandle(STD_INPUT_HANDLE, nullptr);

                HANDLE outputHandle = nullptr;
                DuplicateHandle(GetCurrentProcess(), reinterpret_cast<HANDLE>(_get_osfhandle(STDOUT_FILENO)), GetCurrentProcess(), &outputHandle,
                                    0, TRUE, DUPLICATE_SAME_ACCESS);
                SetStdHandle(STD_OUTPUT_HANDLE, outputHandle);

                HANDLE errorHandle = nullptr;
                DuplicateHandle(GetCurrentProcess(), reinterpret_cast<HANDLE>(_get_osfhandle(STDERR_FILENO)), GetCurrentProcess(), &errorHandle,
                                    0, TRUE, DUPLICATE_SAME_ACCESS);
                SetStdHandle(STD_ERROR_HANDLE, errorHandle);

            }
        }
    }

    ArgumentsPackageWindows args{
        .hInstance = hInstance,
        .hPrevInstance = hPrevInstance,
        .lpCommandLine = commandLineString.data(),
        .nCmdShow = nShow
    };

    auto handle = LoadLibrary(L"UnityPlayer.dll");
    if(!handle)
        throw std::runtime_error("Failed to load UnityPlayer.dll");

    rebindModuleImport(handle, "KERNEL32.dll", replacementKernelFunctions,
                    sizeof(replacementKernelFunctions) / sizeof(replacementKernelFunctions[0]), true);

    UnityMainPtr = reinterpret_cast<UnityMainType>(GetProcAddress(handle, "UnityMain"));
    if(!UnityMainPtr)
        throw std::runtime_error("Failed to find UnityMain in UnityPlayer.dll");

    if(!gameEarlyInit())
        return 1;

    return translator_main(gameMainWindows, &args);
}

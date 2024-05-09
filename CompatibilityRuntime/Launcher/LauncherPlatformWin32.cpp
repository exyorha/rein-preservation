#include "LauncherPlatformWin32.h"

#include <filesystem>

const WNDCLASSEX LauncherPlatformWin32::m_windowClass = {
    .cbSize = sizeof(m_windowClass),
    .style = CS_DBLCLKS | CS_HREDRAW | CS_OWNDC | CS_VREDRAW,
    .lpfnWndProc = windowProc,
    .cbClsExtra = 0,
    .cbWndExtra = 0,
    .hInstance = reinterpret_cast<HINSTANCE>(&__ImageBase),
    .hIcon = nullptr,
    .hCursor = LoadCursor(nullptr, IDC_ARROW),
    .hbrBackground = nullptr,
    .lpszMenuName = nullptr,
    .lpszClassName = L"NieRLauncher",
    .hIconSm = nullptr
};

LauncherPlatformWin32::LauncherPlatformWin32(LauncherApplicationInterface *application) : LauncherPlatform(application), m_redraw(true),
    m_postingInput(false) {

    windowClass.classAtom = RegisterClassEx(&m_windowClass);
    if(windowClass.classAtom == 0)
        throw std::runtime_error("RegisterClassEx has failed");

    font.font = nk_gdifont_create(nullptr, 0);
    if(!font.font)
        throw std::runtime_error("nk_gdifont_create has failed");

    window.hwnd = CreateWindowEx(
        WS_EX_APPWINDOW | WS_EX_OVERLAPPEDWINDOW,
        MAKEINTATOM(windowClass.classAtom),
        L"Launcher",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        nullptr,
        nullptr,
        reinterpret_cast<HINSTANCE>(&__ImageBase),
        this);
    if(!window.hwnd)
        throw std::runtime_error("CreateWindowEx has failed");

    RECT rect;
    GetClientRect(window.hwnd, &rect);

    ctx = nk_gdi_init(&context.context, font.font, GetDC(window.hwnd), rect.right - rect.left, rect.bottom - rect.top);
    if(!ctx)
        throw std::runtime_error("nk_gdi_init has failed");
}


nk_context *LauncherPlatformWin32::nuklear() const {
    return ctx;
}

struct nk_rect LauncherPlatformWin32::screenRect() const {
    unsigned int width, height;

    nk_gdi_get_size(context.context, &width, &height);

    return nk_rect(0, 0, width, height);
}

LauncherPlatformWin32::~LauncherPlatformWin32() = default;

int LauncherPlatformWin32::run() {
    MSG msg;

    while(GetMessage(&msg, nullptr, 0, 0)) {
        do {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) && msg.message != WM_QUIT);

        if(m_postingInput) {
            m_postingInput = false;
            nk_input_end(ctx);
        }

        if(msg.message == WM_QUIT)
            break;

        if(m_redraw && window.hwnd) {


        }
    }

    return msg.wParam;
}

LRESULT LauncherPlatformWin32::windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LauncherPlatformWin32 *platform = nullptr;

    if(uMsg == WM_CREATE) {
        auto create = reinterpret_cast<CREATESTRUCT *>(lParam);

        platform = static_cast<LauncherPlatformWin32 *>(create->lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(platform));
    } else if(hWnd) {
        platform = reinterpret_cast<LauncherPlatformWin32 *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if(platform && hWnd && hWnd == platform->window.hwnd) {

        if(uMsg == WM_DESTROY) {
            platform->window.hwnd = nullptr;

            PostQuitMessage(0);
        } else {
            if(uMsg == WM_PAINT) {
                if(platform->m_postingInput) {
                    platform->m_postingInput = false;
                    nk_input_end(platform->ctx);
                }


                platform->update();

                nk_end(platform->ctx);
#ifndef NK_GDI_DRAW_ON_SCREEN
                nk_gdi_render(platform->context.context, nk_color(0, 0, 0, 0));
#endif

            } else {
                if(!platform->m_postingInput) {
                    platform->m_postingInput = true;
                    nk_input_begin(platform->ctx);
                }
            }

            auto result = nk_gdi_handle_event(platform->context.context, hWnd, uMsg, wParam, lParam);
            if(result) {
                if(uMsg != WM_PAINT) {
                    //platform->m_redraw = true;

                    InvalidateRect(platform->window.hwnd, nullptr, FALSE);

                }

                return 0;
            }

            if(uMsg == WM_SIZE) {
                platform->m_redraw = true;
            }
        }
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

auto LauncherPlatformWin32::queryResolutionList() const -> std::vector<Resolution> {
    std::vector<Resolution> resolutions;

    DEVMODE mode;
    ZeroMemory(&mode, sizeof(mode));
    mode.dmSize = sizeof(mode);
    mode.dmDriverExtra = 0;

    DWORD modeIndex = 0;
    while(EnumDisplaySettings(nullptr, modeIndex, &mode)) {
        bool existing = false;
        for(const auto &resolution:resolutions) {
            if(resolution.first == mode.dmPelsWidth && resolution.second == mode.dmPelsHeight) {
                existing = true;
                break;
            }
        }

        if(!existing) {
            resolutions.emplace_back(mode.dmPelsWidth, mode.dmPelsHeight);
        }

        modeIndex++;
    }

    return resolutions;
}

std::unique_ptr<LauncherPlatform> createLauncherPlatform(LauncherApplicationInterface *application) {
    return std::make_unique<LauncherPlatformWin32>(application);
}

void LauncherPlatformWin32::startGameWithCommandLine(std::vector<std::string> &commandLine) {
    std::vector<std::wstring> wideArguments;
    wideArguments.reserve(commandLine.size());

    for(const auto &arg: commandLine) {
        if(arg.empty())
            wideArguments.emplace_back();
        else {
            auto length = MultiByteToWideChar(CP_UTF8, 0, arg.data(), arg.size(), nullptr, 0);
            if(length == 0)
                throw std::runtime_error("MultiByteToWideChar has failed");

            std::wstring argument;
            argument.resize(length);
            length = MultiByteToWideChar(CP_UTF8, 0, arg.data(), arg.size(), argument.data(), argument.size());
            if(length == 0)
                throw std::runtime_error("MultiByteToWideChar has failed");

            wideArguments.emplace_back(std::move(argument));
        }
    }

    std::vector<wchar_t> modulePathChars(PATH_MAX);
    DWORD outLength;
    DWORD error;

    do {
        outLength = GetModuleFileName(nullptr, modulePathChars.data(), modulePathChars.size());
        if(outLength == 0) {
            throw std::runtime_error("GetModuleFileName failed");
        }

        if(error == ERROR_INSUFFICIENT_BUFFER) {
            modulePathChars.resize(modulePathChars.size() * 2);
        }

    } while(error == ERROR_INSUFFICIENT_BUFFER);

    auto executablePath = (std::filesystem::path(modulePathChars.begin(), modulePathChars.end()).parent_path() / "NieR.exe").wstring();

    std::vector<wchar_t *> argumentPointers;
    argumentPointers.reserve(commandLine.size() + 2);
    argumentPointers.emplace_back(executablePath.data());

    for(auto &arg: wideArguments) {
        argumentPointers.emplace_back(arg.data());
    }

    argumentPointers.emplace_back(nullptr);

    _wexecv(executablePath.c_str(), argumentPointers.data());
}

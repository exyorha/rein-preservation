#ifndef LAUNCHER_PLATFORM_WIN32_H
#define LAUNCHER_PLATFORM_WIN32_H

#include <windows.h>

#include "LauncherApplicationInterface.h"
#include "LauncherPlatform.h"

extern "C" {
    extern unsigned char __ImageBase;
}

class LauncherPlatformWin32 final : public LauncherPlatform {
public:
    explicit LauncherPlatformWin32(LauncherApplicationInterface *application);
    ~LauncherPlatformWin32() override;

    nk_context *nuklear() const override;
    int run() override;
    struct nk_rect screenRect() const override;

    std::vector<Resolution> queryResolutionList() const override;
    void startGameWithCommandLine(std::vector<std::string> &commandLine) override;

    std::filesystem::path dataPath() const override;
    void openDirectory(const std::filesystem::path &path) const override;

private:
    static LRESULT CALLBACK windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    static const WNDCLASSEX m_windowClass;

    struct WindowClass {
        ATOM classAtom = 0;

        ~WindowClass() {
            if(classAtom != 0) {
                UnregisterClass(MAKEINTATOM(classAtom), reinterpret_cast<HINSTANCE>(&__ImageBase));
            }
        }
    } windowClass;

    struct FontHolder {
        GdiFont *font = nullptr;

        ~FontHolder() {
            if(font) {
                nk_gdifont_del(font);
            }
        }
    } font;

    struct Window {
        HWND hwnd = nullptr;

        ~Window() {
            if(hwnd)
                DestroyWindow(hwnd);
        }
    } window;

    struct NuklearContext {
        nk_gdi_ctx context = nullptr;

        ~NuklearContext() {
            if(context) {
                nk_gdi_shutdown(context);
            }
        }
    } context;


    struct nk_context* ctx;
    bool m_redraw;
    bool m_postingInput;
};

#endif


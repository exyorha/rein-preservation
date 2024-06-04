#ifndef LAUNCHER_PLATFORM_X11_H
#define LAUNCHER_PLATFORM_X11_H

#include "LauncherPlatform.h"

#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

class LauncherPlatformX11 final : public LauncherPlatform {
public:
    LauncherPlatformX11(LauncherApplicationInterface *application);
    ~LauncherPlatformX11() override;

    nk_context *nuklear() const override;

    int run() override;

    struct nk_rect screenRect() const override;

    std::vector<Resolution> queryResolutionList() const override;
    void startGameWithCommandLine(std::vector<std::string> &commandLine) override;

    std::filesystem::path dataPath() const override;
    void openDirectory(const std::filesystem::path &path) const override;

private:
    struct DisplayHandle {
        Display *display = nullptr;

        ~DisplayHandle() {
            if(display) {
                XCloseDisplay(display);
            }
        }
    } display;

    XFont *font;
    int screen;
    Window window;
    nk_context *ctx;
};

#endif

#include <WebView/CEFWebViewImplementation.h>
#include <GLES/SDL/SDLWrapper.h>
#include <GLES/SDL/RealSDLSymbols.h>
#include <filesystem>
#include <stdexcept>

static SDL_Window *getWebViewWindow() {
    auto window = getSDLWindowForWebView();
    if(!window)
        throw std::runtime_error("No SDL window suitable for the web view");

    return window;
}

float CEFWebViewImplementation::screenWidth() {
    int width, height;
    RealSDLSymbols::getSingleton().realGetWindowSize(getWebViewWindow(), &width, &height);
    printf("SDL reports window size as (%d,%d)\n", width, height);
    return width;
}

float CEFWebViewImplementation::screenHeight() {
    int width, height;
    RealSDLSymbols::getSingleton().realGetWindowSize(getWebViewWindow(), &width, &height);
    printf("SDL reports window size as (%d,%d)\n", width, height);
    return height;
}

int64_t CEFWebViewImplementation::getParentWindowHandle() {

    SDL_SysWMinfo wmInfo;
    memset(&wmInfo, 0, sizeof(wmInfo));
    wmInfo.version.major = 2;
    wmInfo.version.minor = 0;

    if(!RealSDLSymbols::getSingleton().realGetWindowWMInfo(getWebViewWindow(), &wmInfo))
        throw std::runtime_error("SDL_GetWindowWMInfo (real) has failed");

    if(wmInfo.subsystem != SDL_SYSWM_X11) {
        throw std::runtime_error("Not running under X11");
    }

    return wmInfo.info.x11.window;
}

std::string CEFWebViewImplementation::getWebViewHostPath() {
    return (std::filesystem::read_symlink("/proc/self/exe").parent_path() / "WebViewHost").string();
}

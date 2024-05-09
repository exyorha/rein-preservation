#include "LauncherPlatformX11.h"
#include <filesystem>

LauncherPlatformX11::LauncherPlatformX11(LauncherApplicationInterface *application) : LauncherPlatform(application) {

    display.display = XOpenDisplay(nullptr);
    if(!display.display)
        throw std::runtime_error("failed to open the default display");

    font = nk_xfont_create(display.display, "");

    auto root = XDefaultRootWindow(display.display);
    screen = XDefaultScreen(display.display);
    auto visual = XDefaultVisual(display.display, screen);
    auto colormap = XCreateColormap(display.display, root, visual, AllocNone);

    XSetWindowAttributes setWindowAttributes;
    setWindowAttributes.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
        ButtonPress | ButtonReleaseMask| ButtonMotionMask |
        Button1MotionMask | Button3MotionMask | Button4MotionMask | Button5MotionMask|
        PointerMotionMask | KeymapStateMask;

    setWindowAttributes.colormap = colormap;

    window = XCreateWindow(display.display, root, 0, 0, 640, 480, 0, XDefaultDepth(display.display, screen), InputOutput, visual, CWEventMask | CWColormap,
                           &setWindowAttributes);

    XStoreName(display.display, window, "Launcher");
    XMapWindow(display.display, window);

    XWindowAttributes attributes;
    XGetWindowAttributes(display.display, window, &attributes);

    ctx = nk_xlib_init(font, display.display, screen, window, visual, colormap, attributes.width, attributes.height);
    if(!ctx)
        throw std::runtime_error("nk_xlib_init has failed");

}

LauncherPlatformX11::~LauncherPlatformX11() {
    nk_xlib_shutdown();
    nk_xfont_del(display.display, font);
}

nk_context *LauncherPlatformX11::nuklear() const {
    return ctx;
}

int LauncherPlatformX11::run() {
    while(true) {

        nk_input_begin(ctx);

        bool redraw = false;

        do {
            XEvent ev;
            XNextEvent(display.display, &ev);
            if(ev.type == ClientMessage) {
                return 0;
            }

            if(!XFilterEvent(&ev, window) && ev.type != NoExpose) {
                redraw = true;
                nk_xlib_handle_event(display.display, screen, window, &ev);
            }
        } while(XQLength(display.display) != 0);

        nk_input_end(ctx);


        if(redraw) {
            update();

            nk_end(ctx);

            nk_xlib_render(window, nk_rgb(0, 0, 0));
        }

    }
}

struct nk_rect LauncherPlatformX11::screenRect() const {

    unsigned int width, height;
    nk_xlib_get_size(&width, &height);

    return nk_rect(0, 0, width, height);
}

auto LauncherPlatformX11::queryResolutionList() const -> std::vector<Resolution> {

    Rotation currentRotation;
    auto supportedRotations = XRRRotations(display.display, screen, &currentRotation);

    int numberOfSizes;
    auto sizes = XRRSizes(display.display, screen, &numberOfSizes);

    std::vector<Resolution> resolutions;
    for(int index = 0; index < numberOfSizes; index++) {
        if(currentRotation & (RR_Rotate_90 | RR_Rotate_270)) {
            resolutions.emplace_back(sizes[index].height, sizes[index].width);
        } else {
            resolutions.emplace_back(sizes[index].width, sizes[index].height);
        }
    }

    if(resolutions.empty()) {
        resolutions.emplace_back(DisplayWidth(display.display, screen), DisplayHeight(display.display, screen));
    }

    return resolutions;
}

std::unique_ptr<LauncherPlatform> createLauncherPlatform(LauncherApplicationInterface *application) {
    return std::make_unique<LauncherPlatformX11>(application);
}

void LauncherPlatformX11::startGameWithCommandLine(std::vector<std::string> &commandLine) {
    auto gameExecutable = (std::filesystem::read_symlink("/proc/self/exe").parent_path() / "NieR").string();

    std::vector<char *> args;
    args.reserve(2 + commandLine.size());

    args.emplace_back(gameExecutable.data());
    for(auto &arg: commandLine) {
        args.emplace_back(arg.data());
    }

    args.emplace_back(nullptr);

    execv(gameExecutable.c_str(), args.data());
}

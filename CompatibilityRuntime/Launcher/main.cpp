#include "LauncherPlatform.h"
#include "LauncherApplication.h"

#ifdef _WIN32

static void unrecoverableError(const char *what) {
    std::string message("An unrecoverable error has occurred and the application will now exit.\n\n");
    message.append(what);

    MessageBoxA(nullptr, message.c_str(), nullptr, MB_OK);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, wchar_t *lpCommandLine, int nShow) {
    try {

        LauncherApplication app;

        return createLauncherPlatform(&app)->run();
    } catch(const std::exception &e) {
        unrecoverableError(e.what());
        return 1;
    } catch(...) {
        unrecoverableError("No details are available.");

        return 1;
    }
}


#else
int main(int argc, char **argv) {
    LauncherApplication app;

    return createLauncherPlatform(&app)->run();
}

#endif

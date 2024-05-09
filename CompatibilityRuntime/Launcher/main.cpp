#include "LauncherPlatform.h"
#include "LauncherApplication.h"

int main(int argc, char **argv) {
    LauncherApplication app;

    return createLauncherPlatform(&app)->run();
}


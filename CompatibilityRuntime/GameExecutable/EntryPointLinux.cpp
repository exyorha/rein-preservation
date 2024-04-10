#include "GameEntryPoint.h"

#include <unity_stub.h>
#include <translator_api.h>

struct LinuxArgumentsPackage {
    int argc;
    char **argv;
};

static int invokeUnityLinux(void *package) {
    auto args = static_cast<LinuxArgumentsPackage *>(package);

    return PlayerMain(args->argc, args->argv);
}

static int gameMainLinux(void *arg) {
    auto args = static_cast<LinuxArgumentsPackage *>(arg);

    return gameMain(args->argc, args->argv, invokeUnityLinux, args);
}

int main(int argc, char **argv) {
    LinuxArgumentsPackage args {
        .argc = argc,
        .argv = argv
    };

    if(!gameEarlyInit())
        return 1;

    return translator_main(gameMainLinux, &args);
}

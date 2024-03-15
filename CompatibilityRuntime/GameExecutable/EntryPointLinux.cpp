#include "GameEntryPoint.h"

#include <unity_stub.h>
#include <translator_api.h>
#include <vector>
#include <cstring>

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

    std::vector<char *> argArray;
    if(args.argc == 0) {
        argArray.emplace_back(strdup("executable"));
    } else {
        argArray.emplace_back(args.argv[0]);
    }

    for(const char *builtinArg: { "-force-glcore", "-force-gfx-without-build" }) {
        argArray.emplace_back(strdup(builtinArg));
    }

    for(int argno = 1; argno < args.argc; argno++) {
        argArray.emplace_back(args.argv[argno]);
    }

    argArray.emplace_back(nullptr);

    args.argc = argArray.size() - 1;
    args.argv = argArray.data();

    return translator_main(gameMainLinux, &args);
}

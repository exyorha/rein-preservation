#include "support.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <stdexcept>

void stubcall(const char *format, ...) {
    va_list args;

    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fflush(stderr);

    abort();
}

std::filesystem::path thisLibraryDirectory() {
        static unsigned char thisImageSymbol;

    Dl_info info;

    if(!dladdr(&thisImageSymbol, &info))
        throw std::runtime_error("Unable to resolve own library");

    return std::filesystem::absolute(std::filesystem::path(info.dli_fname)).parent_path();
}

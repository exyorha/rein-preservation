#include <cstdio>

#include "conversion_context.h"

int main(int argc, char **argv) {
    if(argc != 3) {
        fprintf(stderr, "Usage: %s <SOURCE APK FILE> <TARGET DIRECTORY>\n", argv[0]);
        return 1;
    }

    ConversionContext context(argv[2]);

    context.processAPK(argv[1]);

    context.finalize();


    return 0;
}

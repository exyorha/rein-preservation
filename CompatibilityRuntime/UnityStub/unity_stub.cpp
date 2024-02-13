#include <unity_stub.h>

#include <stdio.h>

#if defined(_WIN32)
int __attribute__((dllexport)) UnityMain(int argc, char **argv)
#else
int PlayerMain(int argc, char **argv)
#endif
{
    fprintf(stderr, "You're attempting to run the stub Unity player library that only exists to be linked to.\n");
    return 1;
}


#include <unity_stub.h>

#include <stdio.h>

#if defined(_WIN32)

int UNITY_STUB_IMPORT UnityMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, const wchar_t *lpCommandLine, int nShow)

#else

int PlayerMain(int argc, char **argv)

#endif
{
    fprintf(stderr, "You're attempting to run the stub Unity player library that only exists to be linked to.\n");
    return 1;
}


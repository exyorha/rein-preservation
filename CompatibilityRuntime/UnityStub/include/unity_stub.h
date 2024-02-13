#ifndef UNITY_STUB_H
#define UNITY_STUB_H

#if defined(_WIN32)

#if defined(UNITY_STUB_BUILD)
#define UNITY_STUB_IMPORT __attribute__((dllexport))
#else
#define UNITY_STUB_IMPORT __attribute__((dllimport))
#endif

    extern "C" {

        int UNITY_STUB_IMPORT UnityMain(int argc, char **argv);

    }

    static inline int PlayerMain(int argc, char **argv) {
        return UnityMain(argc, argv);
    }

#else

    int PlayerMain(int argc, char **argv);

#endif

#endif

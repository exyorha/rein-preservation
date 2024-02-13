#include <Bionic/BionicSyscalls.h>
#include <Bionic/BionicCallouts.h>

#include <cerrno>
#include <fcntl.h>

#include "GlobalContext.h"

int plat_open(const char* path, int oflag, int mode) {
    int flags = translateOpenFlag(oflag);

    if(flags < 0) {
        bionic_set_errno(BIONIC_EINVAL);
        return -1;
    }


    auto result = open(path, flags, mode);
    if(result < 0) {
        bionic_set_errno(translateErrno(errno));
    }

    return result;
}

ssize_t plat_readlink(const char *pathname,
                        char *buf, size_t bufsiz) {

    if(strcmp(pathname, "/proc/1/exe") == 0) {
        /*
         * Querying the path to the executable.
         */

        auto path = GlobalContext::get().il2cpp().path().u8string();

        strncpy(buf, reinterpret_cast<const char *>(path.c_str()), bufsiz);

        printf("plat_readlink: reading /proc/1/exe, returned: %.*s\n", static_cast<int>(bufsiz), buf);

        return std::min<size_t>(path.size() + 1, bufsiz);

    } else {

        auto result = readlink(pathname, buf, bufsiz);
        if(result < 0)
            bionic_set_errno(translateErrno(errno));

        return result;
    }
}

int plat_stat(const char *path, struct bionic_stat *statbuf) {
    struct stat64 native;

    auto result = ::stat64(path, &native);
    if(result >= 0) {
        translateStat(&native, statbuf);
    }

    return result;
}

int plat_lstat(const char *path, struct bionic_stat *statbuf) {
    struct stat64 native;

    auto result = ::stat64(path, &native);
    if(result >= 0) {
        translateStat(&native, statbuf);
    }

    return result;
}

int plat_access(const char *path, int amode) {
    auto result = access(path, translateAccessMode(amode));

    if(result < 0)
        bionic_set_errno(translateErrno(errno));

    return result;
}


char *plat_getcwd(char *buf, size_t size) {
    auto result = getcwd(buf, size);
    if(result == nullptr) {
        bionic_set_errno(translateErrno(errno));
    }

    return result;
}

int plat_rename(const char *oldpath, const char *newpath) {
    auto result = rename(oldpath, newpath);
    if(result < 0)
        bionic_set_errno(translateErrno(errno));

    return result;
}

int plat_unlink(const char *pathname) {
    auto result = unlink(pathname);
    if(result < 0)
        bionic_set_errno(translateErrno(errno));

    return result;
}

int plat_rmdir(const char *pathname) {
    auto result = rmdir(pathname);
    if(result < 0)
        bionic_set_errno(translateErrno(errno));

    return result;
}

int plat_mkdir(const char *pathname, bionic_mode_t mode) {
    auto result = mkdir(pathname, mode);
    if(result < 0)
        bionic_set_errno(translateErrno(errno));

    return result;
}

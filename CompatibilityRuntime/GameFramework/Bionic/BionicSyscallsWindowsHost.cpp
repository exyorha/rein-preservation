#include <Bionic/BionicSyscalls.h>
#include <Bionic/BionicCallouts.h>
#include <Windows/StringConversions.h>

#include <ELF/Image.h>

#include <cerrno>
#include <cstdio>

#include <io.h>

std::wstring windowsPathFromAndroidPath(const std::string_view &path) {

    /*
     * This path could be a result of incorrect resolution of a relative
     * path. If it is, then it'll contain multiple drive specifications.
     * Keep only the last one, and strip everything else.
     *
     * This also repairs the cases like '/C:/' that we produce in
     * androidPathFromWindowsPath below.
     */

    std::string_view adjustedPath(path);

    auto colonIndex = adjustedPath.find_last_of(':');
    if(colonIndex != std::string::npos && colonIndex != 0) {
        adjustedPath = adjustedPath.substr(colonIndex - 1);
    }

    /*
     * We let mingw to do the rest of normalization. It does it well enough.
     */
    auto converted = utf8ToWide(adjustedPath);

    printf("Converted Android path to Windows path: '%.*s' -> '%ls'\n",
           static_cast<int>(path.size()), path.data(),
           converted.c_str());

    return converted;

}

std::string androidPathFromWindowsPath(const std::wstring_view &path) {
    auto converted = wideToUtf8(path);
    /*
     * Add a leading slash.
     */
    converted.insert(converted.begin(), '/');

    /*
     * Convert slashes.
     */
    for(auto &ch: converted) {
        if(ch == '\\') {
            ch = '/';
        }
    }

    printf("Converted Windows path to Android path: '%.*ls' -> '%s'\n",
           static_cast<int>(path.size()), path.data(),
           converted.c_str());

    return converted;
}

int plat_open(const char* path, int oflag, int mode) {

    auto convPath = windowsPathFromAndroidPath(path);

    int flags = translateOpenFlag(oflag);

    if(flags < 0) {
        bionic_set_errno(BIONIC_EINVAL);
        return -1;
    }


    auto result = _wopen(convPath.c_str(), flags, mode);
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

        auto path = androidPathFromWindowsPath(GlobalContext::get().linkingSet().mainImage()->path().wstring());

        auto bytesToCopy = std::min<size_t>(path.size(), bufsiz);
        memcpy(buf, path.data(), bytesToCopy);

        printf("plat_readlink: reading /proc/1/exe, returned: %.*s\n", static_cast<int>(bufsiz), buf);

        return bytesToCopy;

    } else {

        fprintf(stderr, "plat_readlink: called on '%s' on Windows, always reporting ENOENT\n", pathname);
        bionic_set_errno(translateErrno(errno));
        return -1;
    }
}

int plat_stat(const char *path, struct bionic_stat *statbuf) {

    auto convPath = windowsPathFromAndroidPath(path);

    struct stat64 native;

    auto result = ::_wstat64(convPath.c_str(), &native);
    if(result >= 0) {
        translateStat(&native, statbuf);
    } else {
        bionic_set_errno(translateErrno(errno));
    }

    return result;
}


int plat_lstat(const char *path, struct bionic_stat *statbuf) {
    /*
     * On Windows, we just treat lstat as the same as regular stat.
     */

    return plat_stat(path, statbuf);
}

int plat_access(const char *path, int amode) {
    auto convPath = windowsPathFromAndroidPath(path);

    auto result = ::_waccess(convPath.c_str(), translateAccessMode(amode));

    if(result < 0)
        bionic_set_errno(translateErrno(errno));

    return result;
}

char *plat_getcwd(char *buf, size_t size) {

    struct ResultHolder {
        wchar_t *path = nullptr;

        ~ResultHolder() {
            if(path)
                free(path);
        }
    } result;

    result.path = ::_wgetcwd(nullptr, 0);
    if(result.path == nullptr) {
        bionic_set_errno(translateErrno(errno));
        return nullptr;
    }

    auto converted = androidPathFromWindowsPath(result.path);

    if(converted.size() + 1 > size) {
        bionic_set_errno(BIONIC_ERANGE);
        return nullptr;
    }

    memcpy(buf, converted.data(), converted.size() + 1);
    return buf;
}

int plat_rename(const char *oldpath, const char *newpath) {
    auto woldpath = windowsPathFromAndroidPath(oldpath);
    auto wnewpath = windowsPathFromAndroidPath(newpath);

    auto result = _wrename(woldpath.c_str(), wnewpath.c_str());
    if(result < 0)
        bionic_set_errno(translateErrno(errno));

    return result;
}

int plat_unlink(const char *pathname) {
    auto wpathname = windowsPathFromAndroidPath(pathname);

    auto result = _wunlink(wpathname.c_str());
    if(result < 0)
        bionic_set_errno(translateErrno(errno));

    return result;
}

int plat_rmdir(const char *pathname) {
    auto wpathname = windowsPathFromAndroidPath(pathname);

    auto result = _wrmdir(wpathname.c_str());
    if(result < 0)
        bionic_set_errno(translateErrno(errno));

    return result;
}

int plat_mkdir(const char *pathname, bionic_mode_t mode) {
    /*
     * Directory mode is irrelevant on Windows.
     */
    (void)mode;

    auto wpathname = windowsPathFromAndroidPath(pathname);

    auto result = _wmkdir(wpathname.c_str());
    if(result < 0)
        bionic_set_errno(translateErrno(errno));

    return result;
}

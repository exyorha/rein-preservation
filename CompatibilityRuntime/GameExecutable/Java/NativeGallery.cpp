#include <Java/NativeGallery.h>
#include <Java/JNIState.h>
#include <Java/JNIString.h>

#include <fstream>
#include <filesystem>

#include <Il2CppUtilities.h>

#ifdef _WIN32
#include <objbase.h>
#include <shellapi.h>
#include <shlobj.h>
#else
#include <pwd.h>
#endif

std::shared_ptr<JNIClass> NativeGallery::makeClass() {
    auto co = std::make_shared<JNIClass>("com/yasirkula/unity/NativeGallery", parent("java/lang/Object"));

    co->registerMethod("CheckPermission", "(Ljava/lang/Object;Z)I", &NativeGallery::CheckPermission);
    co->registerMethod("CheckPermission", "(Landroid/content/Context;Z)I", &NativeGallery::CheckPermission);
    co->registerMethod("SaveMedia", "(Landroid/content/Context;ILjava/lang/String;Ljava/lang/String;)Ljava/lang/String;",
                       &NativeGallery::SaveMedia);

    return co;
}

int32_t NativeGallery::CheckPermission(const std::shared_ptr<JNIObject> &context, bool readPermission) {
    printf("NativeGallery::CheckPermission\n");

    return 1; /* Granted */
}

std::shared_ptr<JNIObject> NativeGallery::SaveMedia(const std::shared_ptr<JNIObject> &androidContext, int32_t mediaType,
                                                    const std::shared_ptr<JNIString> &filename,
                                                    const std::shared_ptr<JNIString> &album) {

    (void)androidContext;
    nonNull(album);
    nonNull(filename);

    printf("NativeGallery::SaveMedia: media type - %d, album - '%s', filename - '%s'\n", mediaType,
           album->string().c_str(), filename->string().c_str());


#ifdef _WIN32
    std::string_view adjustedPath(filename->string());

    auto colonIndex = adjustedPath.find_last_of(':');
    if(colonIndex != std::string::npos && colonIndex != 0) {
        adjustedPath = adjustedPath.substr(colonIndex - 1);
    }

    std::filesystem::path sourceFile(utf8ToUtf16(adjustedPath));
#else
    std::filesystem::path sourceFile(filename->string());
#endif

    auto targetFile = getBasePictureDirectory() / album->string() / sourceFile.filename();

#ifdef _WIN32
    printf("NativeGallery::SaveMedia: moving from %ls to %ls\n", sourceFile.c_str(), targetFile.c_str());
#else
    printf("NativeGallery::SaveMedia: moving from %s to %s\n", sourceFile.c_str(), targetFile.c_str());
#endif

    std::filesystem::create_directories(targetFile.parent_path());

    std::filesystem::copy_file(sourceFile, targetFile);

#ifdef _WIN32
    std::string targetGamePath = utf16ToUtf8(targetFile.u16string());

    /*
     * Add a leading slash.
     */
    targetGamePath.insert(targetGamePath.begin(), '/');

    /*
     * Convert slashes.
     */
    for(auto &ch: targetGamePath) {
        if(ch == '\\') {
            ch = '/';
        }
    }

#else
    std::string targetGamePath = targetFile.string();
#endif

    return std::make_shared<JNIString>(std::move(targetGamePath));
}

#ifdef _WIN32

std::filesystem::path NativeGallery::getBasePictureDirectory() {
    struct StringFreer {
        PWSTR string = nullptr;

        ~StringFreer() {
            if(string)
                CoTaskMemFree(string);
        }
    } path;

    auto result = SHGetKnownFolderPath(FOLDERID_Pictures, KF_FLAG_CREATE, nullptr, &path.string);
    if(FAILED(result))
        throw std::runtime_error("SHGetKnownFolderPath has failed");

    return path.string;
}

#else

const std::filesystem::path &NativeGallery::getHomeDirectory(std::optional<std::filesystem::path> &cachedHome) {
    if(!cachedHome.has_value()) {
        auto home = getenv("HOME");
        if(home) {
            cachedHome.emplace(home);
        } else {
            auto pwentry = getpwuid(getuid());
            if(!pwentry)
                throw std::runtime_error("no XDG_CONFIG_HOME, HOME, or passwd entry for the current user");

            cachedHome.emplace(pwentry->pw_dir);
        }
    }

    return *cachedHome;
}

std::filesystem::path NativeGallery::getConfigDirectory(std::optional<std::filesystem::path> &cachedHome) {
    std::filesystem::path path;

    auto config = getenv("XDG_CONFIG_HOME");
    if(config) {
        path = config;
    } else {
        path = getHomeDirectory(cachedHome) / ".config";
    }

    return path;
}


std::filesystem::path NativeGallery::getBasePictureDirectory() {
    std::optional<std::filesystem::path> home;

    try {
        return getXDGDirectory("XDG_PICTURES_DIR=", home);

    } catch(const std::exception &e) {
        fprintf(stderr, "Failed to get the Pictures directory from user-dirs.dir: %s\nUsing the fallback path.\n", e.what());

        return getHomeDirectory(home) / "Pictures";
    }
}

std::filesystem::path NativeGallery::getXDGDirectory(const std::string_view &key, std::optional<std::filesystem::path> &cachedHome) {
    std::ifstream stream;
    stream.exceptions(std::ios::failbit | std::ios::badbit | std::ios::eofbit);
    stream.open(getConfigDirectory(cachedHome) / "user-dirs.dirs", std::ios::in | std::ios::binary);

    std::string line;
    while(std::getline(stream, line)) {
        if(line.starts_with(key)) {
            std::string_view quotedString = std::string_view(line).substr(key.size());

            if(!quotedString.starts_with('"'))
                throw std::runtime_error("unquoted string in user-dirs.dirs");

            quotedString = quotedString.substr(1);

            std::string unquoted;
            unquoted.reserve(quotedString.size());

            bool doOneMoreLine;
            bool finished = false;

            do {
                doOneMoreLine = false;

                bool escaped = false;
                for(char ch: quotedString) {
                    if(escaped) {
                        /*
                        * We process backslashes by the POSIX rules (2.2.3 Double-Quotes)
                        */
                        if(ch == '\\' || ch == '"' || ch == '$' || ch == '`') {
                            unquoted.push_back(ch);
                        } else {
                            unquoted.push_back('\\');
                            unquoted.push_back(ch);
                        }
                        escaped = false;
                    } else if(ch == '\\') {
                        escaped = true;
                    } else if(ch == '"') {
                        /*
                        * End of string, finish parsing.
                        */

                        finished = true;
                        break;
                    } else {
                        unquoted.push_back(ch);
                    }
                }

                if(escaped) {
                    doOneMoreLine = true;

                    std::getline(stream, line);
                    quotedString = line;
                }
            } while(doOneMoreLine);

            if(!finished)
                throw std::runtime_error("didn't reach the end quote");

            /*
             * the XDG spec specifies that the string must start with one of
             * the two exact strings: '$HOME/' for a home-relative path, or
             * '/' for an absolute path.'
             */
            static const std::string_view homePrefix = "$HOME/";
            if(unquoted.starts_with(homePrefix)) {
                return getHomeDirectory(cachedHome) / unquoted.substr(homePrefix.size());
            } else if(!unquoted.starts_with('/')) {
                throw std::runtime_error("XDG user dir path is neither HOME-relative nor absolute");
            } else {
                return unquoted;
            }
        }
    }

    /*
     * Should not be reachable - should throw the EOF from std::getline
     */
    throw std::runtime_error("the requested key was not found");
}

#endif

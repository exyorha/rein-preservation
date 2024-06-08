#ifndef JAVA_NATIVE_GALLERY_H
#define JAVA_NATIVE_GALLERY_H

#include <Java/JNIClass.h>
#include <filesystem>
#include <optional>

class JNIString;

class NativeGallery final : public JNIObject {
public:

    static std::shared_ptr<JNIClass> makeClass();

    static int32_t CheckPermission(const std::shared_ptr<JNIObject> &context, bool readPermission);

    static std::shared_ptr<JNIObject> SaveMedia(const std::shared_ptr<JNIObject> &androidContext, int32_t mediaType,
                                                const std::shared_ptr<JNIString> &album,
                                                const std::shared_ptr<JNIString> &filename);

private:
    static std::filesystem::path getBasePictureDirectory();
#ifndef _WIN32
    static const std::filesystem::path & getHomeDirectory(std::optional<std::filesystem::path> &cachedHome);
    static std::filesystem::path getXDGDirectory(const std::string_view &key, std::optional<std::filesystem::path> &cachedHome);

    static std::filesystem::path getConfigDirectory(std::optional<std::filesystem::path> &cachedHome);
#endif
};

#endif




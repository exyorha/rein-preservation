#include "Octo.h"
#include "OctoContentStorage.h"
#include "Il2CppUtilities.h"
#include "translator_api.h"
#include "ZIPBasedStorage.h"

#include <cstdio>
#include <cinttypes>

using GetPersistentDataPathPtr = Il2CppString *(*)();
static GetPersistentDataPathPtr getPersistentDataPath;

static int32_t Octo_Caching_OctoBaseCaching_IsCached(Il2CppObject *this_, Il2CppString *fileName, Il2CppString *md5, int64_t fileSize, void *original) {

    auto fileNameUtf8 = stringToUtf8(fileName);
    auto md5Utf8 = stringToUtf8(md5);

    auto result = contentStorageInstance->locateFile(fileNameUtf8, md5Utf8, fileSize);
    if(!result.has_value()) {
        fprintf(stderr, "Octo.Caching.OctoBaseCaching.IsCached: the requested file with object name '%s', md5 '%s' and file size %" PRId64 " was not found in the storage\n",
                fileNameUtf8.c_str(), md5Utf8.c_str(), fileSize);

        return 0;
    }

    return 3; // means 'cached, latest version'
}


static Il2CppString *Octo_Caching_OctoBaseCaching_GetStorageFilePath(Il2CppObject *this_, Il2CppString *fileName, Il2CppString *md5, void *original) {

    auto fileNameUtf8 = stringToUtf8(fileName);
    auto md5Utf8 = stringToUtf8(md5);

    auto result = contentStorageInstance->locateFile(fileNameUtf8, md5Utf8);
    if(!result.has_value()) {
        fprintf(stderr, "Octo.Caching.OctoBaseCaching.GetStorageFilePath: the requested file with object name '%s', md5 '%s' was not found in the storage\n",
                fileNameUtf8.c_str(), md5Utf8.c_str());
        return nullptr;
    }

    return stringFromUtf8(result->string());
}

static Il2CppString *Octo_Util_FileUtil_GetAndroidOctoRoot(Il2CppString *(*original)(void)) {
    auto dataPath = stringToUtf8(getPersistentDataPath());
    dataPath.append("/octo");

    return stringFromUtf8(dataPath);
}

std::filesystem::path getGamePersistentDataPath() {
    auto dataPath = stringToUtf8(getPersistentDataPath());

    return std::u8string_view(reinterpret_cast<const char8_t *>(dataPath.data()), dataPath.size());
}

static Il2CppArray *Octo_Loader_OctoAPI_DecryptAES(Il2CppObject *this_, Il2CppArray *input, void *original) {
    (void)this_;

    return input;
}

static Il2CppObject *Adam_Framework_Resource_DarkOctoSetupper_AssetBundleLoadInterceptor(Il2CppString *abName, Il2CppString *path, void *original) {
    (void)abName;
    (void)path;
    (void)original;

    /*
     * The return value is UnityEngine.AssetBundleCreateRequest; returning null
     * selects the default behavior of loading the bundle from the file at path.
     */
    return nullptr;
}

static Il2CppObject *UnityEngine_AssetBundle_LoadFromFileAsync_Internal(Il2CppString *path, uint32_t crc, uint64_t offset,
    Il2CppObject *(*original)(Il2CppString *path, uint32_t crc, uint64_t offset)) {

#ifdef _WIN32
    auto widePath = stringToUtf16(path);
    std::filesystem::path fsPath(reinterpret_cast<const wchar_t *>(widePath.data()), reinterpret_cast<const wchar_t *>(widePath.data() + widePath.size()));
#else
    auto u8Path = stringToUtf8(path);
    std::filesystem::path fsPath(u8Path.data(), u8Path.data() + u8Path.size());
#endif

    auto location = ZIPBasedStorage::unwrapZIPPath(std::move(fsPath));
    if(location.has_value()) {

        offset += location->fileOffset;

#ifdef _WIN32
        auto wstr = location->zipFilePath.wstring();
        path = stringFromUtf16(
            std::u16string_view(reinterpret_cast<const char16_t *>(wstr.data()), reinterpret_cast<const char16_t *>(wstr.data() + wstr.size())));
#else
        path = stringFromUtf8(location->zipFilePath.string());
#endif
    }

    return original(path, crc, offset);
}

INTERPOSE_ICALL("UnityEngine.AssetBundle::LoadFromFileAsync_Internal", UnityEngine_AssetBundle_LoadFromFileAsync_Internal);

void InitializeOcto() {

    translator_divert_method("Octo.dll::Octo.Caching.OctoBaseCaching::IsCached",
                             Octo_Caching_OctoBaseCaching_IsCached);

    translator_divert_method("Octo.dll::Octo.Caching.OctoBaseCaching::GetStorageFilePath",
                             Octo_Caching_OctoBaseCaching_GetStorageFilePath);

    translator_divert_method("Octo.dll::Octo.Util.FileUtil::GetAndroidOctoRoot",
                             Octo_Util_FileUtil_GetAndroidOctoRoot);

    /*
     * Easier than implementing the encryption in the server.
     */
    translator_divert_method("Octo.dll::Octo.Loader.OctoAPI::DecryptAes",
                             Octo_Loader_OctoAPI_DecryptAES);

    /*
     * We don't need this interceptor, since we unmask the assets at the build time.
     * It also interferes with the zip loading.
     */

    translator_divert_method("Assembly-CSharp.dll::Adam.Framework.Resource.DarkOctoSetupper::AssetBundleLoadInterceptor",
                             Adam_Framework_Resource_DarkOctoSetupper_AssetBundleLoadInterceptor);

    getPersistentDataPath = reinterpret_cast<GetPersistentDataPathPtr>(translator_resolve_native_icall("UnityEngine.Application::get_persistentDataPath()"));

}

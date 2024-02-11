#include "Octo.h"
#include "OctoContentStorage.h"
#include "Il2CppUtilities.h"
#include "translator_api.h"

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

void InitializeOcto() {

    translator_divert_method("Octo.dll::Octo.Caching.OctoBaseCaching::IsCached",
                             Octo_Caching_OctoBaseCaching_IsCached);

    translator_divert_method("Octo.dll::Octo.Caching.OctoBaseCaching::GetStorageFilePath",
                             Octo_Caching_OctoBaseCaching_GetStorageFilePath);

    translator_divert_method("Octo.dll::Octo.Util.FileUtil::GetAndroidOctoRoot",
                             Octo_Util_FileUtil_GetAndroidOctoRoot);

    getPersistentDataPath = reinterpret_cast<GetPersistentDataPathPtr>(translator_resolve_native_icall("UnityEngine.Application::get_persistentDataPath()"));

}

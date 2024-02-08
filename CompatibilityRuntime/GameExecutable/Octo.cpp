#include "Octo.h"
#include "OctoContentStorage.h"
#include "Il2CppUtilities.h"

#include <cstdio>
#include <cinttypes>

int32_t Octo_Caching_OctoBaseCaching_IsCached(Il2CppObject *this_, Il2CppString *fileName, Il2CppString *md5, int64_t fileSize, void *original) {

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


Il2CppString *Octo_Caching_OctoBaseCaching_GetStorageFilePath(Il2CppObject *this_, Il2CppString *fileName, Il2CppString *md5, void *original) {

    auto fileNameUtf8 = stringToUtf8(fileName);
    auto md5Utf8 = stringToUtf8(md5);

    auto result = contentStorageInstance->locateFile(fileNameUtf8, md5Utf8);
    if(!result.has_value()) {
        fprintf(stderr, "Octo.Caching.OctoBaseCaching.GetStorageFilePath: the requested file with object name '%s', md5 '%s' was not found in the storage\n",
                fileNameUtf8.c_str(), md5Utf8.c_str());
        return nullptr;
    }

    auto newpathString = result->string();
    return il2cpp_string_new_len(newpathString.data(), newpathString.size());
}

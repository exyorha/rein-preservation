#include "Octo.h"
#include "OctoContentStorage.h"
#include "Il2CppUtilities.h"

#include <cstdio>
#include <cinttypes>

bool Octo_Caching_OctoAppCaching_IsInApp(Il2CppObject *this_, Il2CppString *string, uint64_t integer, MD5Value md5,
                                                bool (*original)(Il2CppObject *this_, Il2CppString *string, uint64_t integer, MD5Value md5)) {

    printf("Octo.Caching.OctoAppCaching.IsInApp(%s, %" PRId64 ", md5: ",
           stringToUtf8(string).c_str(), integer);

    for(auto byte: md5.hash) {
        printf("%02x", byte);
    }

    printf("): ");

    fflush(stdout);

    auto result = original(this_, string, integer, md5);

    printf("the original says %d\n", result);

    return result;
}

int32_t Octo_Caching_OctoBaseCaching_IsCached(Il2CppObject *this_, Il2CppString *fileName, Il2CppString *md5, int64_t fileSize, void *original) {

    auto fileNameUtf8 = stringToUtf8(fileName);
    auto md5Utf8 = stringToUtf8(md5);

    printf("Octo.Caching.OctoBaseCaching.IsCached(%s, %s, %" PRId64 ")\n",
        fileNameUtf8.c_str(),
        md5Utf8.c_str(),
        fileSize);

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

    printf("Octo.Caching.OctoBaseCaching.GetStorageFilePath(%s, %s)\n",
        fileNameUtf8.c_str(), md5Utf8.c_str());

    /*
     * TODO: in the future we probably want to give the file paths *directly*
     * so it won't go through UnityWebRequest, but right now we don't want to
     * risk Octo scribbling over our content files.
     */
    auto result = contentStorageInstance->locateFile(fileNameUtf8, md5Utf8);
    if(!result.has_value()) {
        fprintf(stderr, "Octo.Caching.OctoBaseCaching.GetStorageFilePath: the requested file with object name '%s', md5 '%s' was not found in the storage\n",
                fileNameUtf8.c_str(), md5Utf8.c_str());
        return nullptr;
    }

    auto newpathString = result->string();
/*
    std::stringstream newpath;
    newpath << "/fakeoctopath/" << stringToUtf8(fileName);
    auto newpathString = newpath.str();*/


    printf("  -> %s for %s\n", newpathString.c_str(), fileNameUtf8.c_str());

    return il2cpp_string_new_len(newpathString.data(), newpathString.size());
}

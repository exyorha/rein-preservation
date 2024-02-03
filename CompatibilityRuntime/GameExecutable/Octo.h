#ifndef OCTO_H
#define OCTO_H

#include <il2cpp-api.h>

struct MD5Value {
    unsigned char hash[16];
};

class OctoContentStorage;

bool Octo_Caching_OctoAppCaching_IsInApp(Il2CppObject *this_, Il2CppString *string, uint64_t integer, MD5Value md5,
                                         bool (*original)(Il2CppObject *this_, Il2CppString *string, uint64_t integer, MD5Value md5));

int32_t Octo_Caching_OctoBaseCaching_IsCached(Il2CppObject *this_, Il2CppString *fileName, Il2CppString *md5, int64_t fileSize, void *original);

Il2CppString *Octo_Caching_OctoBaseCaching_GetStorageFilePath(Il2CppObject *this_, Il2CppString *fileName, Il2CppString *md5,
                                                                     void *original);


extern OctoContentStorage *contentStorageInstance;

#endif

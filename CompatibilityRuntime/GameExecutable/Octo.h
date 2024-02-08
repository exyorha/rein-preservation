#ifndef OCTO_H
#define OCTO_H

#include <il2cpp-api.h>

struct MD5Value {
    unsigned char hash[16];
};

class OctoContentStorage;

int32_t Octo_Caching_OctoBaseCaching_IsCached(Il2CppObject *this_, Il2CppString *fileName, Il2CppString *md5, int64_t fileSize, void *original);

Il2CppString *Octo_Caching_OctoBaseCaching_GetStorageFilePath(Il2CppObject *this_, Il2CppString *fileName, Il2CppString *md5,
                                                                     void *original);


extern OctoContentStorage *contentStorageInstance;

#endif

#include "FastAES.h"

#include <stdexcept>
#include <vector>

#include <windows.h>
#include <bcrypt.h>
#include <ntdef.h>

struct BCryptContext {
    BCRYPT_ALG_HANDLE handle = nullptr;
    ~BCryptContext() {
        if(handle) {
            BCryptCloseAlgorithmProvider(handle, 0);
        }
    }
};

struct BCryptKey {
    BCRYPT_KEY_HANDLE handle = nullptr;
    ~BCryptKey() {
        if(handle) {
            BCryptDestroyKey(handle);
        }
    }
};

template<typename T>
static void getBCryptProperty(BCRYPT_HANDLE object, LPCWSTR property, T &value) {
    ULONG written;

    auto result = BCryptGetProperty(object, property, reinterpret_cast<PUCHAR>(&property), sizeof(property), &written, 0);
    if(!NT_SUCCESS(result))
        throw std::runtime_error("BCryptGetProperty has failed");

    if(written != sizeof(T))
        throw std::runtime_error("unexpected length of a BCrypt property");
}

template<typename T>
static void setBCryptProperty(BCRYPT_HANDLE object, LPCWSTR property, const T &value) {
    auto result = BCryptSetProperty(object, property, const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(&value)), sizeof(value), 0);
    if(!NT_SUCCESS(result))
        throw std::runtime_error("BCryptSetProperty has failed");
}

#if 0
static std::vector<unsigned char> makeBlob(Il2CppArray *in) {

    auto length = il2cpp_array_length(in);
    auto arrayHeaderSize = il2cpp_array_object_header_size();

    std::vector<unsigned char> blob(sizeof(BCRYPT_KEY_DATA_BLOB_HEADER) + length);
    auto header = reinterpret_cast<BCRYPT_KEY_DATA_BLOB_HEADER *>(blob.data());

    header->dwMagic = BCRYPT_KEY_DATA_BLOB_MAGIC;
    header->dwVersion = BCRYPT_KEY_DATA_BLOB_VERSION1;
    header->cbKeyData = length;

    memcpy(blob.data() + sizeof(BCRYPT_KEY_DATA_BLOB_HEADER),
           reinterpret_cast<unsigned char *>(in) + arrayHeaderSize, length);

    return blob;
}
#endif

Il2CppArray *FastAES_NativeDecrypt(Il2CppObject *decryptor,
                                   int32_t paddingMode,
                                   Il2CppArray *inputData,
                                   int32_t inputOffset,
                                   int32_t inputLength,
                                   Il2CppArray *keyArray,
                                   Il2CppArray *ivArray,
                                   void *unusedOriginal) {

    if(!inputData || !keyArray || !ivArray) {
        fprintf(stderr, "FastAES_NativeDecrypt: one of the input parameters is null\n");
        return nullptr;
    }

    int32_t inputDataLength = il2cpp_array_length(inputData);

    if(inputOffset < 0 || inputLength < 0 || inputDataLength < inputOffset + inputLength) {
        fprintf(stderr, "FastAES_NativeDecrypt: supplied offset and length are out of range for the input array\n");
        return nullptr;
    }

    auto arrayHeaderSize = il2cpp_array_object_header_size();

    BCryptContext context;

    auto result = BCryptOpenAlgorithmProvider(&context.handle, BCRYPT_AES_ALGORITHM, nullptr, 0);
    if(!NT_SUCCESS(result))
        throw std::runtime_error("BCryptOpenAlgorithmProvider has failed");

    DWORD keyObjectLength;

    getBCryptProperty(context.handle, BCRYPT_OBJECT_LENGTH, keyObjectLength);

    setBCryptProperty(context.handle, BCRYPT_CHAINING_MODE, BCRYPT_CHAIN_MODE_CBC);

    std::vector<UCHAR> keyObject(keyObjectLength);
    BCryptKey key;

    {
#if 0
        auto keyBlob = makeBlob(keyArray);
        result = BCryptImportKey(context.handle, nullptr, BCRYPT_KEY_DATA_BLOB, &key.handle, keyObject.data(), keyObject.size(),
                                 keyBlob.data(), keyBlob.size(), 0);
        if(!NT_SUCCESS(result))
            throw std::runtime_error("BCryptImportKey has failed");
#endif


        result = BCryptGenerateSymmetricKey(context.handle, &key.handle, keyObject.data(), keyObject.size(),
                                                 reinterpret_cast<unsigned char *>(keyArray) + arrayHeaderSize,
                                                 il2cpp_array_length(keyArray), 0);
        if(!NT_SUCCESS(result))
            throw std::runtime_error("BCryptGenerateSymmetricKey has failed");
    }

    std::vector<unsigned char> runningIV(
        reinterpret_cast<unsigned char *>(ivArray) + arrayHeaderSize,
        reinterpret_cast<unsigned char *>(ivArray) + arrayHeaderSize + il2cpp_array_length(ivArray));

    ULONG outputLength, unusedFinalOutputLength;
    result = BCryptDecrypt(
        key.handle,
        reinterpret_cast<unsigned char *>(inputData) + arrayHeaderSize + inputOffset, inputDataLength,
        nullptr,
        runningIV.data(), runningIV.size(),
        nullptr, 0, &outputLength,
        BCRYPT_BLOCK_PADDING);

    if(!NT_SUCCESS(result))
        throw std::runtime_error("BCryptDecrypt has failed");

    auto outputArray = il2cpp_array_new(il2cpp_class_get_element_class(il2cpp_object_get_class(reinterpret_cast<Il2CppObject *>(inputData))),
                                                                       outputLength);

    runningIV.assign(reinterpret_cast<unsigned char *>(ivArray) + arrayHeaderSize,
                     reinterpret_cast<unsigned char *>(ivArray) + arrayHeaderSize + runningIV.size());

    result = BCryptDecrypt(
        key.handle,
        reinterpret_cast<unsigned char *>(inputData) + arrayHeaderSize + inputOffset, inputDataLength,
        nullptr,
        runningIV.data(), runningIV.size(),
        reinterpret_cast<unsigned char *>(outputArray) + arrayHeaderSize, outputLength, &unusedFinalOutputLength,
        BCRYPT_BLOCK_PADDING);
    if(!NT_SUCCESS(result))
        throw std::runtime_error("BCryptDecrypt has failed");

    return outputArray;
}

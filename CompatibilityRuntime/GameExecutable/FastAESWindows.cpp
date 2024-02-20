#include "FastAES.h"
#include "Il2CppUtilities.h"

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

Il2CppArray *FastAES_NativeDecrypt(Il2CppObject *decryptor,
                                   int32_t paddingMode,
                                   Il2CppArray *inputDataPtr,
                                   int32_t inputOffset,
                                   int32_t inputLength,
                                   Il2CppArray *keyArrayPtr,
                                   Il2CppArray *ivArrayPtr,
                                   void *unusedOriginal) {

    if(!inputDataPtr || !keyArrayPtr || !ivArrayPtr) {
        fprintf(stderr, "FastAES_NativeDecrypt: one of the input parameters is null\n");
        return nullptr;
    }

    ArrayWrapper<unsigned char> inputData(inputDataPtr);
    ArrayWrapper<unsigned char> keyArray(keyArrayPtr);
    ArrayWrapper<unsigned char> ivArray(ivArrayPtr);

    if(inputOffset < 0 || inputLength < 0 || inputData.size() < inputOffset + inputLength) {
        fprintf(stderr, "FastAES_NativeDecrypt: supplied offset and length are out of range for the input array\n");
        return nullptr;
    }

    BCryptContext context;

    auto result = BCryptOpenAlgorithmProvider(&context.handle, BCRYPT_AES_ALGORITHM, nullptr, 0);
    if(!NT_SUCCESS(result))
        throw std::runtime_error("BCryptOpenAlgorithmProvider has failed");

    DWORD keyObjectLength;

    getBCryptProperty(context.handle, BCRYPT_OBJECT_LENGTH, keyObjectLength);

    setBCryptProperty(context.handle, BCRYPT_CHAINING_MODE, BCRYPT_CHAIN_MODE_CBC);

    std::vector<UCHAR> keyObject(keyObjectLength);
    BCryptKey key;

    result = BCryptGenerateSymmetricKey(context.handle, &key.handle, keyObject.data(), keyObject.size(),
                                        keyArray.data(),
                                        keyArray.size(), 0);
    if(!NT_SUCCESS(result))
        throw std::runtime_error("BCryptGenerateSymmetricKey has failed");

    std::vector<unsigned char> runningIV(keyArray.data(), keyArray.data() + keyArray.size());

    ULONG outputLength, unusedFinalOutputLength;
    result = BCryptDecrypt(
        key.handle,
        &inputData[inputOffset], inputLength,
        nullptr,
        runningIV.data(), runningIV.size(),
        nullptr, 0, &outputLength,
        BCRYPT_BLOCK_PADDING);

    if(!NT_SUCCESS(result))
        throw std::runtime_error("BCryptDecrypt has failed");

    ArrayWrapper<unsigned char> outputArray(il2cpp_array_new(il2cpp_class_get_element_class(il2cpp_object_get_class(inputData.object())),
                                                                       outputLength));

    runningIV.assign(keyArray.data(), keyArray.data() + keyArray.size());

    result = BCryptDecrypt(
        key.handle,
        &inputData[inputOffset], inputLength,
        nullptr,
        runningIV.data(), runningIV.size(),
        outputArray.data(), outputArray.size(), &unusedFinalOutputLength,
        BCRYPT_BLOCK_PADDING);
    if(!NT_SUCCESS(result))
        throw std::runtime_error("BCryptDecrypt has failed");

    return outputArray.array();
}

#include "FastAES.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <memory>
#include <vector>

#include <openssl/evp.h>

Il2CppArray *FastAES_NativeDecrypt(Il2CppObject *decryptor,
                                   int32_t paddingMode,
                                   Il2CppArray *inputData,
                                   int32_t inputOffset,
                                   int32_t inputLength,
                                   Il2CppArray *key,
                                   Il2CppArray *iv,
                                   void *unusedOriginal) {
    (void)decryptor;
    (void)unusedOriginal;

    if(!inputData || !key || !iv) {
        fprintf(stderr, "FastAES_NativeDecrypt: one of the input parameters is null\n");
        return nullptr;
    }

    int32_t inputDataLength = il2cpp_array_length(inputData);

    if(inputOffset < 0 || inputLength < 0 || inputDataLength < inputOffset + inputLength) {
        fprintf(stderr, "FastAES_NativeDecrypt: supplied offset and length are out of range for the input array\n");
        return nullptr;
    }

    auto keyLengthBytes = il2cpp_array_length(key);

    const EVP_CIPHER *cipher;
    if(keyLengthBytes == 16) {
        cipher = EVP_aes_128_cbc();
    } else if(keyLengthBytes == 32) {
        cipher = EVP_aes_256_cbc();
    } else {
        fprintf(stderr, "FastAES_NativeDecrypt: bad key length\n");
        return nullptr;
    }

    auto ivLengthBytes = il2cpp_array_length(iv);
    if(ivLengthBytes != 16) {
        fprintf(stderr, "FastAES_NativeDecrypt: bad IV length\n");
        return nullptr;
    }

    struct ContextDeleter {
        inline void operator()(EVP_CIPHER_CTX *ctx) const {
            EVP_CIPHER_CTX_free(ctx);
        }
    };

    std::unique_ptr<EVP_CIPHER_CTX, ContextDeleter> ctx(EVP_CIPHER_CTX_new());
    if(!ctx) {
        fprintf(stderr, "FastAES_NativeDecrypt: EVP_CIPHER_CTX_new has failed\n");
        return nullptr;
    }

    auto arrayHeaderSize = il2cpp_array_object_header_size();
    auto keyData = reinterpret_cast<uint8_t *>(key) + arrayHeaderSize;
    auto ivData = reinterpret_cast<uint8_t *>(iv) + arrayHeaderSize;

    if(!EVP_DecryptInit_ex(ctx.get(), cipher, nullptr, keyData, ivData)) {
        fprintf(stderr, "FastAES_NativeDecrypt: EVP_DecryptInit_ex has failed\n");
        return nullptr;
    }

    std::vector<unsigned char> output(((inputLength + 15) & ~15) + 16);

    int outputLength;
    if(!EVP_DecryptUpdate(ctx.get(), output.data(), &outputLength,
        reinterpret_cast<uint8_t *>(inputData) + arrayHeaderSize + inputOffset,
        inputLength)) {
        fprintf(stderr, "FastAES_NativeDecrypt: EVP_DecryptUpdate has failed\n");
        return nullptr;
    }

    int finalLen;
    if(!EVP_DecryptFinal(ctx.get(), output.data() + outputLength, &finalLen)) {
        fprintf(stderr, "FastAES_NativeDecrypt: EVP_DecryptFinal has failed\n");
        return nullptr;
    }

    outputLength += finalLen;

    auto outputArray = il2cpp_array_new(il2cpp_class_get_element_class(il2cpp_object_get_class(reinterpret_cast<Il2CppObject *>(inputData))),
                                                                       outputLength);
    memcpy(reinterpret_cast<unsigned char *>(outputArray) + arrayHeaderSize, output.data(), outputLength);

    return outputArray;
}

#include "FastAES.h"
#include "Il2CppUtilities.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <memory>
#include <vector>

#include <openssl/evp.h>

Il2CppArray *FastAES_NativeDecrypt(Il2CppObject *decryptor,
                                   int32_t paddingMode,
                                   Il2CppArray *inputDataPtr,
                                   int32_t inputOffset,
                                   int32_t inputLength,
                                   Il2CppArray *keyPtr,
                                   Il2CppArray *ivPtr,
                                   void *unusedOriginal) {
    (void)decryptor;
    (void)unusedOriginal;

    if(!inputDataPtr || !keyPtr || !ivPtr) {
        fprintf(stderr, "FastAES_NativeDecrypt: one of the input parameters is null\n");
        return nullptr;
    }

    ArrayWrapper<uint8_t> inputData(inputDataPtr);
    ArrayWrapper<uint8_t> key(keyPtr);
    ArrayWrapper<uint8_t> iv(ivPtr);

    if(inputOffset < 0 || inputLength < 0 || inputData.size() < inputOffset + inputLength) {
        fprintf(stderr, "FastAES_NativeDecrypt: supplied offset and length are out of range for the input array\n");
        return nullptr;
    }

    const EVP_CIPHER *cipher;
    if(key.size() == 16) {
        cipher = EVP_aes_128_cbc();
    } else if(key.size() == 32) {
        cipher = EVP_aes_256_cbc();
    } else {
        fprintf(stderr, "FastAES_NativeDecrypt: bad key length\n");
        return nullptr;
    }

    if(iv.size() != 16) {
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

    if(!EVP_DecryptInit_ex(ctx.get(), cipher, nullptr, key.data(), iv.data())) {
        fprintf(stderr, "FastAES_NativeDecrypt: EVP_DecryptInit_ex has failed\n");
        return nullptr;
    }

    std::vector<unsigned char> output(((inputLength + 15) & ~15) + 16);

    int outputLength;
    if(!EVP_DecryptUpdate(ctx.get(), output.data(), &outputLength,
        &inputData[inputOffset],
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

    auto outputArray = il2cpp_array_new(il2cpp_class_get_element_class(il2cpp_object_get_class(inputData.object())),
                                                                       outputLength);
    memcpy(ArrayWrapper<unsigned char>(outputArray).data(), output.data(), outputLength);

    return outputArray;
}

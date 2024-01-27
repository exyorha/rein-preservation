#include "frontend.h"
#include "aes.h"

#include <string.h>
#include <stdlib.h>
#include <android/log.h>

static char *bufferToHex(const unsigned char *data, size_t dataSize) {
    if(data == NULL)
        return strdup("<NULL>");

    size_t hexLength = dataSize * 2 + 1;

    char *output = malloc(hexLength);

    const unsigned char *ptr = data;
    const unsigned char *limit = ptr + dataSize;

    static const char hextable[] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
    };

    char *outputPtr = output;

    while(ptr < limit) {
        unsigned char byte = *ptr++;

        *outputPtr++ = hextable[byte >> 4];
        *outputPtr++ = hextable[byte & 0x0F];
    }

    *outputPtr = 0;

    return output;
}

static int AES_CBC_encrypt_impl(unsigned char *output, unsigned int unused_possiblyOutputLength,
    const unsigned char *input, unsigned int inputLength,
    const unsigned char *key,
    unsigned int keyLengthInBits,
    const unsigned char *initializationVector,
    unsigned char paddingMode) {

    mbedtls_aes_context ctx;
    mbedtls_aes_init(&ctx);

    mbedtls_aes_setkey_enc(&ctx, key, keyLengthInBits);

    unsigned int blockAlignedInput = inputLength & ~15;
    unsigned char runningIV[16];
    memcpy(runningIV, initializationVector, 16);

    mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_ENCRYPT, blockAlignedInput, runningIV, input, output);
    unsigned int encryptedBytes = blockAlignedInput;

    input += encryptedBytes;
    output += encryptedBytes;

    if(paddingMode == 2) {
        unsigned char paddingBlock[16];
        memcpy(paddingBlock, input, inputLength & 15);
        memset(&paddingBlock[inputLength & 15], 16 - (inputLength & 15), 16 - (inputLength & 15));
        mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_ENCRYPT, 16, runningIV, paddingBlock, output);
        encryptedBytes += 16;
    }

    mbedtls_aes_free(&ctx);

    return encryptedBytes;
}

static int AES_CBC_decrypt_impl(unsigned char *output,
    const unsigned char *input, unsigned int inputLength,
    const unsigned char *key,
    unsigned int keyLengthInBits,
    const unsigned char *initializationVector,
    unsigned char paddingMode) {

    if((inputLength & 0x0F) || inputLength < 16)
        return 0;

    mbedtls_aes_context ctx;
    mbedtls_aes_init(&ctx);

    mbedtls_aes_setkey_dec(&ctx, key, keyLengthInBits);
    unsigned char runningIV[16];
    memcpy(runningIV, initializationVector, 16);

    mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_DECRYPT, inputLength, runningIV, input, output);

    unsigned int bytesDecrypted = inputLength;

    if(paddingMode == 2) {
        unsigned char paddingLength = output[inputLength - 1];
        if(paddingLength < 1 || paddingLength > 16)
            return 0;

        unsigned char *padding = &output[inputLength - paddingLength];
        unsigned char *limit = &output[inputLength - 1];

        while(padding < limit) {
            if(*padding++ != paddingLength)
                return 0;
        }

        bytesDecrypted -= paddingLength;
    }

    return bytesDecrypted;
}

int AES_CBC_encrypt(
    unsigned char *output, unsigned int unused_possiblyOutputLength,
    const unsigned char *input, unsigned int inputLength,
    const unsigned char *key,
    unsigned int keyLengthInBits,
    const unsigned char *initializationVector,
    unsigned char paddingMode) {

    mbedtls_aes_context ctx;


    int encryptedBytes = AES_CBC_encrypt_impl(output, unused_possiblyOutputLength, input, inputLength,
                                              key, keyLengthInBits, initializationVector, paddingMode);

    char *outputString = bufferToHex(output, encryptedBytes);
    char *inputString = bufferToHex(input, inputLength);
    char *keyString = bufferToHex(key, keyLengthInBits / 8);
    char *ivString = bufferToHex(initializationVector, 16);

    __android_log_print(ANDROID_LOG_DEFAULT, "LogAES", "encrypt: out %s in %s key %s IV %s padding %u",
                        outputString,
                        inputString,
                        ivString,
                        keyString,
                        paddingMode);

    free(outputString);
    free(inputString);
    free(keyString);
    free(ivString);

    return encryptedBytes;
}

int LOGAES_EXPORT AES_CBC_decrypt(
    unsigned char *output,
    const unsigned char *input, unsigned int inputLength,
    const unsigned char *key,
    unsigned int keyLengthInBits,
    const unsigned char *initializationVector,
    unsigned char paddingMode) {

    int decryptedBytes = AES_CBC_decrypt_impl(output, input, inputLength,
                                              key, keyLengthInBits, initializationVector, paddingMode);

    char *outputString = bufferToHex(output, decryptedBytes);
    char *inputString = bufferToHex(input, inputLength);
    char *keyString = bufferToHex(key, keyLengthInBits / 8);
    char *ivString = bufferToHex(initializationVector, 16);

    __android_log_print(ANDROID_LOG_DEFAULT, "LogAES", "decrypt: out %s in %s key %s IV %s padding %u",
                        outputString,
                        inputString,
                        ivString,
                        keyString,
                        paddingMode);

    free(outputString);
    free(inputString);
    free(keyString);
    free(ivString);

    return decryptedBytes;
}

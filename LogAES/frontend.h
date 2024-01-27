#ifndef AES_H
#define AES_H

#include <aes_export.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*
 * Padding mode 2 chooses PKCS#7 padding, any other value selects no padding.
 *
 * Returns the number of bytes produced, including padding). On decryption,
 * returns 0 if the padding is incorrect.
 */
int LOGAES_EXPORT AES_CBC_encrypt(
    unsigned char *output, unsigned int unused_possiblyOutputLength,
    const unsigned char *input, unsigned int inputLength,
    const unsigned char *key,
    unsigned int keyLengthInBits,
    const unsigned char *initializationVector,
    unsigned char paddingMode);

int LOGAES_EXPORT AES_CBC_decrypt(
    unsigned char *output,
    const unsigned char *input, unsigned int inputLength,
    const unsigned char *key,
    unsigned int keyLengthInBits,
    const unsigned char *initializationVector,
    unsigned char paddingMode);

#if defined(__cplusplus)
}
#endif

#endif

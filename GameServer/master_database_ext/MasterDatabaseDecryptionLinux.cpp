#include "MasterDatabaseDecryption.h"

#include <openssl/evp.h>

#include <stdexcept>
#include <iostream>
#include <memory>

namespace MasterDatabase {

    std::vector<char> readAndDecryptFile(std::istream &stream, const unsigned char *key, const unsigned char *iv) {
        std::vector<char> data;

        stream.seekg(0, std::ios::end);
        auto inputSize = static_cast<size_t>(stream.tellg());

        data.reserve(inputSize + 32);

        stream.seekg(0);

        std::vector<char> block(16384);

        stream.exceptions(std::ios::badbit);
        size_t bytes;

        struct ContextDeleter {
            inline void operator()(EVP_CIPHER_CTX *ctx) const {
                EVP_CIPHER_CTX_free(ctx);
            }
        };

        std::unique_ptr<EVP_CIPHER_CTX, ContextDeleter> ctx(EVP_CIPHER_CTX_new());
        if(!ctx)
            throw std::runtime_error("EVP_CIPHER_CTX_new has failed");

        if(!EVP_DecryptInit_ex(ctx.get(), EVP_aes_128_cbc(), nullptr, key, iv))
            throw std::runtime_error("EVP_DecryptInit_ex has failed");

        do {
            stream.read(block.data(), block.size());

            bytes = stream.gcount();

            if(bytes != 0) {
                auto oldLen = data.size();

                data.resize(data.size() + bytes + 16);

                int outLen;
                if(!EVP_DecryptUpdate(ctx.get(),
                    reinterpret_cast<unsigned char *>(data.data()) + oldLen,
                    &outLen,
                    reinterpret_cast<const unsigned char *>(block.data()),
                    bytes))

                    throw std::runtime_error("EVP_DecryptUpdate has failed");

                data.resize(oldLen + outLen);
            }

        } while(bytes != 0);

        int outLen;

        auto oldLen = data.size();
        data.resize(data.size() + 16);

        if(!EVP_DecryptFinal(
            ctx.get(),
            reinterpret_cast<unsigned char *>(data.data()) + oldLen,
            &outLen))
            throw std::runtime_error("EVP_DecryptFinal has failed");

        data.resize(oldLen + outLen);

        return data;
    }
}

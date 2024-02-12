#include "DatabaseImage.h"

#include <fstream>
#include <memory>

#include <openssl/evp.h>

namespace MasterDatabase {

    const unsigned char DatabaseImage::m_key[16] = {
        '6', 'C', 'b', '0', '1', '3', '2', '1', 'E', 'E', '5', 'e', '6', 'b', 'B', 'e'
    };

    const unsigned char DatabaseImage::m_iv[16] = {
        'E' ,'f' ,'c', 'A', 'e', 'f', '4', 'C', 'A', 'e', '5', 'f', '6', 'D', 'a', 'A'
    };

    DatabaseImage::DatabaseImage(const std::filesystem::path &filename) {

        std::ifstream stream;
        stream.exceptions(std::ios::badbit | std::ios::eofbit | std::ios::failbit);
        stream.open(filename, std::ios::in | std::ios::binary);

        stream.seekg(0, std::ios::end);
        auto inputSize = static_cast<size_t>(stream.tellg());

        m_data.reserve(inputSize + 32);

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

        if(!EVP_DecryptInit_ex(ctx.get(), EVP_aes_128_cbc(), nullptr, m_key, m_iv))
            throw std::runtime_error("EVP_DecryptInit_ex has failed");

        do {
            stream.read(block.data(), block.size());

            bytes = stream.gcount();

            if(bytes != 0) {
                auto oldLen = m_data.size();

                m_data.resize(m_data.size() + bytes + 16);

                int outLen;
                if(!EVP_DecryptUpdate(ctx.get(),
                    reinterpret_cast<unsigned char *>(m_data.data()) + oldLen,
                    &outLen,
                    reinterpret_cast<const unsigned char *>(block.data()),
                    bytes))

                    throw std::runtime_error("EVP_DecryptUpdate has failed");

                m_data.resize(oldLen + outLen);
            }

        } while(bytes != 0);

        int outLen;

        auto oldLen = m_data.size();
        m_data.resize(m_data.size() + 16);

        if(!EVP_DecryptFinal(
            ctx.get(),
            reinterpret_cast<unsigned char *>(m_data.data()) + oldLen,
            &outLen))
            throw std::runtime_error("EVP_DecryptFinal has failed");

        m_data.resize(oldLen + outLen);
    }

    DatabaseImage::~DatabaseImage() = default;

}


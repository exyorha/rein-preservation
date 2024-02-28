#include "MasterDatabaseDecryption.h"

#include <windows.h>
#include <bcrypt.h>
#include <ntdef.h>

#include <iostream>
#include <array>

namespace MasterDatabase {

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

    std::vector<char> readAndDecryptFile(std::istream &stream, const unsigned char *keyBytes, const unsigned char *iv) {
        std::vector<char> data;

        stream.seekg(0, std::ios::end);
        auto inputSize = static_cast<size_t>(stream.tellg());

        data.reserve(inputSize + 32);

        stream.seekg(0);

        std::vector<char> block(16384);

        stream.exceptions(std::ios::badbit);
        size_t bytes;

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
                                            const_cast<PUCHAR>(keyBytes),
                                            16, 0);
        if(!NT_SUCCESS(result))
            throw std::runtime_error("BCryptGenerateSymmetricKey has failed");

        std::array<unsigned char, 16> runningIV;
        memcpy(runningIV.data(), iv, runningIV.size());

        bool finalized = false;

        while(true) {
            stream.read(block.data(), block.size());

            bytes = stream.gcount();

            if(finalized) {
                if(bytes == 0)
                    break;
                else
                    throw std::logic_error("more data has been read from the file after the decryption was finalized");
            } else {
                auto oldLen = data.size();

                data.resize(data.size() + bytes + 32);

                bool finalize = bytes != block.size();
                ULONG flags = 0;
                if(finalize) {
                    flags |= BCRYPT_BLOCK_PADDING;
                    finalized = true;
                }

                ULONG outputLength = 0;
                result = BCryptDecrypt(
                    key.handle,
                    reinterpret_cast<PUCHAR>(block.data()), bytes,
                    nullptr,
                    runningIV.data(), runningIV.size(),
                    reinterpret_cast<unsigned char *>(data.data()) + oldLen, data.size() - oldLen, &outputLength,
                    flags);

                if(!NT_SUCCESS(result))
                    throw std::runtime_error("BCryptDecrypt has failed");

                data.resize(oldLen + outputLength);
            }
        }

        return data;
    }
}

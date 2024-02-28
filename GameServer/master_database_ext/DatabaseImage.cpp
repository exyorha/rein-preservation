#include "DatabaseImage.h"
#include "MasterDatabaseDecryption.h"

#include <fstream>
#include <memory>

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

        m_data = readAndDecryptFile(stream, m_key, m_iv);
    }

    DatabaseImage::~DatabaseImage() = default;

}


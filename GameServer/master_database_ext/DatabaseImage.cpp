#include "DatabaseImage.h"

#include <fstream>

namespace MasterDatabase {


    DatabaseImage::DatabaseImage(const std::filesystem::path &filename) {

        std::ifstream stream;
        stream.exceptions(std::ios::badbit | std::ios::eofbit | std::ios::failbit);
        stream.open(filename, std::ios::in | std::ios::binary);

        stream.seekg(0, std::ios::end);
        auto inputSize = static_cast<size_t>(stream.tellg());

        m_data.resize(inputSize);

        stream.seekg(0);

        stream.read(m_data.data(), m_data.size());
    }

    DatabaseImage::~DatabaseImage() = default;

}


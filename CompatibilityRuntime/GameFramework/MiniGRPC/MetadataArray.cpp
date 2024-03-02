#include "MetadataArray.h"

namespace minigrpc {

    MetadataArray::MetadataArray(size_t capacity) {
        m_metadata.reserve(capacity);
    }

    MetadataArray::~MetadataArray() = default;

    void MetadataArray::add(std::string &&key, std::string &&value) {
        m_metadata.emplace_back(std::move(key), std::move(value));
    }
}


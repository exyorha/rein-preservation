#ifndef METADATA_ARRAY_H
#define METADATA_ARRAY_H

#include <vector>
#include <utility>
#include <string>

namespace minigrpc {

    using Metadata = std::pair<std::string, std::string>;

    class MetadataArray {
    public:
        explicit MetadataArray(size_t capacity = 0);
        ~MetadataArray();

        MetadataArray(const MetadataArray &other) = delete;
        MetadataArray &operator =(const MetadataArray &other) = delete;

        inline const std::vector<Metadata> &metadata() const {
            return m_metadata;
        }

        inline void clear() {
            m_metadata.clear();
        }

        void add(std::string &&key, std::string &&value);

    private:
        std::vector<Metadata> m_metadata;
    };

}

#endif

#ifndef BATCH_CONTEXT_H
#define BATCH_CONTEXT_H

#include <string>
#include <variant>
#include "MetadataArray.h"

namespace minigrpc {

    class BatchContext {
    public:
        BatchContext();
        ~BatchContext();

        BatchContext(const BatchContext &other) = delete;
        BatchContext &operator =(const BatchContext &other) = delete;

        void reset();

        void fail(std::string &&error);

        bool failed() const;
        const std::string &error() const;

        inline MetadataArray &metadata() {
            return m_metadata;
        }

        inline const MetadataArray &metadata() const {
            return m_metadata;
        }

        size_t responseSize() const;
        std::vector<unsigned char> *responseData();
        void replaceResponseData(std::vector<unsigned char> &&data);

    private:
        using Failure = std::string;

        struct Success {
        };

        std::variant<std::monostate, Failure, Success> m_result;
        MetadataArray m_metadata;
        std::vector<unsigned char> m_responseData;
        bool m_sliceConsumed;
    };

}

#endif

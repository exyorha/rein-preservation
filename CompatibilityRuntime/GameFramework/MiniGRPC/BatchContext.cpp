#include "BatchContext.h"

#include <cstdio>

namespace minigrpc {

    BatchContext::BatchContext() {
        printf("BatchContext(%p): created\n", this);
    }

    BatchContext::~BatchContext() {
        printf("BatchContext(%p): destroyed\n", this);
    }

    void BatchContext::reset() {
        m_result.emplace<std::monostate>();
        m_responseData.clear();
        m_sliceConsumed = false;
    }

    void BatchContext::fail(std::string &&error) {
        m_result.emplace<Failure>(std::move(error));
    }

    bool BatchContext::failed() const {
        return std::holds_alternative<Failure>(m_result);
    }

    const std::string &BatchContext::error() const {
        return std::get<Failure>(m_result);
    }


    size_t BatchContext::responseSize() const {
        return m_responseData.size();
    }

    std::vector<unsigned char> *BatchContext::responseData() {
        if(m_sliceConsumed)
            return nullptr;

        m_sliceConsumed = true;
        return &m_responseData;
    }

    void BatchContext::replaceResponseData(std::vector<unsigned char> &&data) {
        m_responseData = std::move(data);
        m_sliceConsumed = false;
    }

}

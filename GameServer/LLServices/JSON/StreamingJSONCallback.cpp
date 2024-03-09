#include <LLServices/JSON/StreamingJSONCallback.h>

namespace LLServices {

    StreamingJSONCallback::StreamingJSONCallback() = default;

    StreamingJSONCallback::~StreamingJSONCallback() = default;

    StreamingJSONCallback::StreamingJSONCallback(const StreamingJSONCallback &other) = default;

    StreamingJSONCallback &StreamingJSONCallback::operator =(const StreamingJSONCallback &other) = default;

    StreamingJSONCallback::StreamingJSONCallback(StreamingJSONCallback &&other) noexcept = default;

    StreamingJSONCallback &StreamingJSONCallback::operator =(StreamingJSONCallback &&other) noexcept = default;

    bool StreamingJSONCallback::null() {
        return true;
    }

    bool StreamingJSONCallback::boolean(bool value) {
        (void)value;
        return true;
    }

    bool StreamingJSONCallback::integer(int64_t value) {
        (void)value;
        return true;
    }

    bool StreamingJSONCallback::doubleValue(double value) {
        (void)value;
        return true;
    }

    bool StreamingJSONCallback::string(const std::string_view &value) {
        (void)value;
        return true;
    }

    bool StreamingJSONCallback::startMap() {
        if(!m_path.empty())
            m_path.push_back('.');

        return true;
    }

    bool StreamingJSONCallback::mapKey(const std::string_view &key) {
        auto dot = m_path.find_last_of('.');
        if(dot == std::string::npos) {
            m_path = key;
        } else {
            m_path.replace(dot + 1, m_path.size() - dot - 1, key);
        }

        return true;
    }

    bool StreamingJSONCallback::endMap() {
        auto dot = m_path.find_last_of('.');

        if(dot == std::string::npos) {
            m_path.clear();
        } else {
            m_path.erase(dot, m_path.size() - dot);
        }

        return true;
    }

    bool StreamingJSONCallback::startArray() {
        m_path.append("[]");

        return true;
    }

    bool StreamingJSONCallback::endArray() {
        m_path.erase(m_path.size() - 2, 2);

        return true;
    }

}

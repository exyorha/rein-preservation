#ifndef LLSERVICES_JSON_STREAMING_JSON_CALLBACK_H
#define LLSERVICES_JSON_STREAMING_JSON_CALLBACK_H

#include <cstdint>
#include <string_view>
#include <string>

namespace LLServices {
    class StreamingJSONParser;

    class StreamingJSONCallback {
    protected:
        StreamingJSONCallback();
        ~StreamingJSONCallback();

        StreamingJSONCallback(const StreamingJSONCallback &other);
        StreamingJSONCallback &operator =(const StreamingJSONCallback &other);

        StreamingJSONCallback(StreamingJSONCallback &&other) noexcept;
        StreamingJSONCallback &operator =(StreamingJSONCallback &&other) noexcept;

    public:
        virtual bool null();
        virtual bool boolean(bool value);
        virtual bool integer(int64_t value);
        virtual bool doubleValue(double value);
        virtual bool string(const std::string_view &value);
        virtual bool startMap();
        virtual bool mapKey(const std::string_view &key);
        virtual bool endMap();
        virtual bool startArray();
        virtual bool endArray();

    protected:
        inline const std::string &path() const {
            return m_path;
        }

    private:
        std::string m_path;
    };
}

#endif


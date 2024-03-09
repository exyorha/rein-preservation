#ifndef LLSERVICES_JSON_STREAMING_JSON_PARSER_H
#define LLSERVICES_JSON_STREAMING_JSON_PARSER_H

#include <string_view>
#include <memory>

#include <yajl_parse.h>

namespace LLServices {
    class StreamingJSONCallback;

    class StreamingJSONParser {
    public:
        explicit StreamingJSONParser(StreamingJSONCallback &callback);
        ~StreamingJSONParser();

        StreamingJSONParser(const StreamingJSONParser &other) = delete;
        StreamingJSONParser &operator =(const StreamingJSONParser &other) = delete;

        bool parseChunk(const std::string_view &chunk);
        bool finish();

    private:
        struct YajlDeleter {
            inline void operator()(yajl_handle handle) {
                yajl_free(handle);
            }
        };

        static const yajl_callbacks m_callbacks;

        [[noreturn]] void handleError();

        static int callbackNull(void * ctx);
        static int callbackBoolean(void * ctx, int boolVal);
        static int callbackInteger(void * ctx, long long integerVal);
        static int callbackDouble(void * ctx, double doubleVal);

        static int callbackString(void * ctx, const unsigned char * stringVal,
                            size_t stringLen);

        static int callbackStartMap(void * ctx);
        static int callbackMapKey(void * ctx, const unsigned char * key, size_t stringLen);
        static int callbackEndMap(void * ctx);

        static int callbackStartArray(void * ctx);
        static int callbackEndArray(void * ctx);

        std::unique_ptr<std::remove_pointer<yajl_handle>::type, YajlDeleter>  m_context;
    };

}

#endif

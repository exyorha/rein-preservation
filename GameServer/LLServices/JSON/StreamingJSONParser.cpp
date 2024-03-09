#include <LLServices/JSON/StreamingJSONParser.h>
#include <LLServices/JSON/StreamingJSONCallback.h>

#include <new>
#include <stdexcept>
#include <cstring>

namespace LLServices {

    const yajl_callbacks StreamingJSONParser::m_callbacks = {
        .yajl_null = callbackNull,
        .yajl_boolean = callbackBoolean,
        .yajl_integer = callbackInteger,
        .yajl_double = callbackDouble,
        .yajl_number = nullptr,
        .yajl_string = callbackString,
        .yajl_start_map = callbackStartMap,
        .yajl_map_key = callbackMapKey,
        .yajl_end_map = callbackEndMap,
        .yajl_start_array = callbackStartArray,
        .yajl_end_array = callbackEndArray
    };

    StreamingJSONParser::StreamingJSONParser(StreamingJSONCallback &callback) {
        m_context.reset(yajl_alloc(&m_callbacks, nullptr, &callback));
        if(!m_context)
            throw std::bad_alloc();
    }

    StreamingJSONParser::~StreamingJSONParser()  = default;

    bool StreamingJSONParser::parseChunk(const std::string_view &chunk) {
        auto result = yajl_parse(m_context.get(), reinterpret_cast<const unsigned char *>(chunk.data()), chunk.size());
        if(result == yajl_status_client_canceled)
            return false;

        if(result == yajl_status_error)
            handleError();

        return true;
    }

    bool StreamingJSONParser::finish() {
        auto result = yajl_complete_parse(m_context.get());
        if(result == yajl_status_client_canceled)
            return false;

        if(result == yajl_status_error)
            handleError();

        return true;
    }

    void StreamingJSONParser::handleError() {
        struct ErrorHolder {
            unsigned char *error = nullptr;
            yajl_handle handle;

            explicit ErrorHolder(yajl_handle handle) : handle(handle) {

            }


            ~ErrorHolder() {
                if(error)
                    yajl_free_error(handle, error);
            }

            std::string_view message() const {
                if(error) {
                    return std::string_view(reinterpret_cast<const char *>(error));
                } else {
                    return std::string_view();
                }
            }
        } holder(m_context.get());

        holder.error = yajl_get_error(m_context.get(), 0, nullptr, 0);

        throw std::runtime_error("StreamingJSONParser: " + std::string(holder.message()));
    }

    int StreamingJSONParser::callbackNull(void * ctx) {
        return static_cast<StreamingJSONCallback *>(ctx)->null();
    }

    int StreamingJSONParser::callbackBoolean(void * ctx, int boolVal) {
        return static_cast<StreamingJSONCallback *>(ctx)->boolean(boolVal != 0);
    }


    int StreamingJSONParser::callbackInteger(void * ctx, long long integerVal) {
        return static_cast<StreamingJSONCallback *>(ctx)->integer(integerVal);

    }

    int StreamingJSONParser::callbackDouble(void * ctx, double doubleVal) {
        return static_cast<StreamingJSONCallback *>(ctx)->doubleValue(doubleVal);
    }

    int StreamingJSONParser::callbackString(void * ctx, const unsigned char * stringVal, size_t stringLen) {
        return static_cast<StreamingJSONCallback *>(ctx)->string(
            std::string_view(reinterpret_cast<const char *>(stringVal), stringLen)
        );
    }

    int StreamingJSONParser::callbackStartMap(void * ctx) {
        return static_cast<StreamingJSONCallback *>(ctx)->startMap();
    }

    int StreamingJSONParser::callbackMapKey(void * ctx, const unsigned char * key, size_t stringLen) {
        return static_cast<StreamingJSONCallback *>(ctx)->mapKey(
            std::string_view(reinterpret_cast<const char *>(key), stringLen)
        );
    }

    int StreamingJSONParser::callbackEndMap(void * ctx) {
        return static_cast<StreamingJSONCallback *>(ctx)->endMap();
    }

    int StreamingJSONParser::callbackStartArray(void * ctx) {
        return static_cast<StreamingJSONCallback *>(ctx)->startArray();
    }

    int StreamingJSONParser::callbackEndArray(void * ctx) {
        return static_cast<StreamingJSONCallback *>(ctx)->endArray();
    }
}

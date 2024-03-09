#ifndef LLSERVICES_JSON_WRITER_H
#define LLSERVICES_JSON_WRITER_H

#include <yajl_gen.h>

#include <type_traits>
#include <memory>
#include <string_view>

namespace LLServices {

    class JSONWriter {
    public:
        JSONWriter();
        ~JSONWriter();

        JSONWriter(const JSONWriter &other) = delete;
        JSONWriter &operator =(const JSONWriter &other) = delete;

        void writeInteger(long long number);
        void writeDouble(double number);
        void writeNumber(const std::string_view &numberRepr);

        void writeString(const std::string_view &string);

        void writeNull();
        void writeBool(bool value);

        void writeMapOpen();
        void writeMapClose();

        void writeArrayOpen();
        void writeArrayClose();

        std::string_view output() const;

        void clear();

        void reset(const char *separator = nullptr);

    private:

        using YajlContext = typename std::remove_pointer<yajl_gen>::type;

        struct YajlDeleter {
            inline void operator()(yajl_gen handle) const {
                yajl_gen_free(handle);
            }
        };

        using YajlContextPtr = std::unique_ptr<YajlContext, YajlDeleter>;

        static inline void checkAndThrow(yajl_gen_status status) {
            if(status != yajl_gen_status_ok) {
                throwException(status);
            }
        }

        [[noreturn]] static void throwException(yajl_gen_status status);

        YajlContextPtr m_context;
    };

}

#endif

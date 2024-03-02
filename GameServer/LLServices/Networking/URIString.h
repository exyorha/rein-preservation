#ifndef LLSERVICES_NETWORKING_URI_STRING_H
#define LLSERVICES_NETWORKING_URI_STRING_H

#include <string>
#include <string_view>

namespace LLServices {
    class URIString;

    URIString uriDecode(const std::string &source, bool decodePlus = false);
    URIString uriEncode(const std::string_view &source, bool encodePlus = false);

    class URIString {
    private:
        URIString(char *ptr, size_t size);

    public:
        URIString() noexcept;

        ~URIString();

        URIString(const URIString &other) = delete;
        URIString &operator =(const URIString &other) = delete;

        URIString(URIString &&other) noexcept;
        URIString &operator =(URIString &&other) noexcept;

        std::string_view string() const noexcept;

        inline operator std::string_view () const noexcept {
            return string();
        }

    private:
        friend URIString uriDecode(const std::string &source, bool decodePlus);
        friend URIString uriEncode(const std::string_view &source, bool encodePlus);

        char *m_ptr;
        size_t m_size;
    };

}

#endif

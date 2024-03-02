#include <LLServices/Networking/URIString.h>

#include <event2/http.h>

#include <cstdlib>
#include <cstring>
#include <utility>

namespace LLServices {

    URIString::URIString() noexcept : m_ptr(nullptr), m_size(0) {

    }

    URIString::URIString(char *ptr, size_t size) : m_ptr(ptr), m_size(size) {

    }

    URIString::~URIString() {
        free(m_ptr);
    }


    URIString::URIString(URIString &&other) noexcept : URIString() {
        *this = std::move(other);
    }

    URIString &URIString::operator =(URIString &&other) noexcept {
        if(this != &other) {
            if(m_ptr) {
                free(m_ptr);
            }

            m_ptr = other.m_ptr;
            m_size = other.m_size;

            other.m_ptr = nullptr;
            other.m_size = 0;
        }

        return *this;
    }

    std::string_view URIString::string() const noexcept {
        if(!m_ptr)
            return std::string_view();
        else
            return std::string_view(m_ptr, m_size);
    }

    URIString uriDecode(const std::string &source, bool decodePlus) {
        if(source.empty())
            return URIString();

        size_t size;
        auto result = evhttp_uridecode(source.c_str(), decodePlus, &size);
        if(result == nullptr)
            throw std::bad_alloc();

        return URIString(result, size);
    }


    URIString uriEncode(const std::string_view &source, bool encodePlus) {
        if(source.empty())
            return URIString();

        auto result = evhttp_uriencode(source.data(), source.size(), encodePlus);

        if(result == nullptr)
            throw std::bad_alloc();

        return URIString(result, strlen(result));
    }
}


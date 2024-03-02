#include <LLServices/Networking/URI.h>

#include <cstring>

namespace LLServices {

    URI::URI() {

        auto uri = evhttp_uri_new();

        if(uri == nullptr)
            throw std::bad_alloc();

        m_uri = EvHTTPURI(uri);
    }

    URI::URI(const std::string &sourceUri) {

        auto uri = evhttp_uri_parse(sourceUri.c_str());

        if(uri == nullptr)
            throw std::bad_alloc();

        m_uri = EvHTTPURI(uri);
    }

    URI::URI(const std::string &sourceUri, unsigned int flags) {

        auto uri = evhttp_uri_parse_with_flags(sourceUri.c_str(), flags);

        if(uri == nullptr)
            throw std::bad_alloc();

        m_uri = EvHTTPURI(uri);
    }

    URI::URI(evhttp_uri *uri) : m_uri(uri) {

    }

    URI::~URI() = default;

    URI::URI(URI &&other) noexcept = default;

    URI &URI::operator = (URI &&other) noexcept = default;

    URI::URI(const URI &other) : URI(other.join()) {

    }

    URI &URI::operator =(const URI &other) {
        return *this = URI(other);
    }

    std::optional<std::string_view> URI::fragment() const {
        auto value = evhttp_uri_get_fragment(m_uri.get());

        if(value)
            return value;
        else
            return {};
    }

    void URI::setFragment(const std::optional<std::string> &value) {
        const char *ptr = nullptr;

        if(value.has_value())
            ptr = value->c_str();

        if(evhttp_uri_set_fragment(m_uri.get(), ptr) < 0)
            throw std::bad_alloc();
    }

    std::optional<std::string_view> URI::host() const {
        auto value = evhttp_uri_get_host(m_uri.get());

        if(value)
            return value;
        else
            return {};
    }

    void URI::setHost(const std::optional<std::string> &value) {
        const char *ptr = nullptr;

        if(value.has_value())
            ptr = value->c_str();

        if(evhttp_uri_set_host(m_uri.get(), ptr) < 0)
            throw std::bad_alloc();
    }

    std::optional<std::string_view> URI::path() const {
        auto value = evhttp_uri_get_path(m_uri.get());

        if(value)
            return value;
        else
            return {};
    }

    void URI::setPath(const std::optional<std::string> &value) {
        const char *ptr = nullptr;

        if(value.has_value())
            ptr = value->c_str();

        if(evhttp_uri_set_path(m_uri.get(), ptr) < 0)
            throw std::bad_alloc();
    }

    std::optional<unsigned int> URI::port() const {
        auto value = evhttp_uri_get_port(m_uri.get());

        if(value < 0)
            return {};
        else
            return static_cast<unsigned int>(value);
    }

    void URI::setPort(const std::optional<unsigned int> &port) {
        int value;

        if(port.has_value())
            value = static_cast<int>(*port);
        else
            value = -1;

        if(evhttp_uri_set_port(m_uri.get(), value) < 0)
            throw std::bad_alloc();
    }

    std::optional<std::string_view> URI::query() const {
        auto value = evhttp_uri_get_query(m_uri.get());

        if(value)
            return value;
        else
            return {};
    }

    void URI::setQuery(const std::optional<std::string> &value) {
        const char *ptr = nullptr;

        if(value.has_value())
            ptr = value->c_str();

        if(evhttp_uri_set_query(m_uri.get(), ptr) < 0)
            throw std::bad_alloc();
    }

    std::optional<std::string_view> URI::scheme() const {
        auto value = evhttp_uri_get_scheme(m_uri.get());

        if(value)
            return value;
        else
            return {};
    }

    void URI::setScheme(const std::optional<std::string> &value) {
        const char *ptr = nullptr;

        if(value.has_value())
            ptr = value->c_str();

        if(evhttp_uri_set_scheme(m_uri.get(), ptr) < 0)
            throw std::bad_alloc();
    }

#ifndef _WIN32
    std::optional<std::string_view> URI::unixSocket() const {
        auto value = evhttp_uri_get_unixsocket(m_uri.get());

        if(value)
            return value;
        else
            return {};
    }

    void URI::setUnixSocket(const std::optional<std::string> &value) {
        const char *ptr = nullptr;

        if(value.has_value())
            ptr = value->c_str();

        if(evhttp_uri_set_unixsocket(m_uri.get(), ptr) < 0)
            throw std::bad_alloc();
    }
#endif

    std::optional<std::string_view> URI::userInfo() const {
        auto value = evhttp_uri_get_userinfo(m_uri.get());

        if(value)
            return value;
        else
            return {};
    }

    void URI::setUserInfo(const std::optional<std::string> &value) {
        const char *ptr = nullptr;

        if(value.has_value())
            ptr = value->c_str();

        if(evhttp_uri_set_userinfo(m_uri.get(), ptr) < 0)
            throw std::bad_alloc();
    }

    void URI::setFlags(unsigned int flags) {
        evhttp_uri_set_flags(m_uri.get(), flags);
    }

    std::string URI::join() const {
        std::string buf;

        char *result;

        do {
            if(buf.empty())
                buf.resize(256);
            else
                buf.resize(buf.size() * 2);

            result = evhttp_uri_join(m_uri.get(), buf.data(), buf.size());

        } while(result == nullptr);

        auto length = strlen(result);
        buf.resize(length);
        buf.shrink_to_fit();

        return buf;
    }
}

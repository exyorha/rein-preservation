#ifndef LLSERVICES_NETWORKING_URI_H
#define LLSERVICES_NETWORKING_URI_H

#include <LLServices/Networking/EventTypes.h>
#include <LLServices/Networking/PossiblyUnownedPointer.h>

#include <string>
#include <optional>
#include <string_view>

namespace LLServices {
    class URI {
    public:
        URI();
        explicit URI(const std::string &uri);
        URI(const std::string &uri, unsigned int flags);
        ~URI();

        /*
         * Creates an URI over an *unowned* evhttp_uri.
         */
        explicit URI(evhttp_uri *object);

        URI(const URI &other);
        URI &operator =(const URI &other);

        URI(URI &&other) noexcept;
        URI &operator = (URI &&other) noexcept;

        std::optional<std::string_view> fragment() const;
        void setFragment(const std::optional<std::string> &fragment = {});

        std::optional<std::string_view> host() const;
        void setHost(const std::optional<std::string> &host = {});

        std::optional<std::string_view> path() const;
        void setPath(const std::optional<std::string> &value = {});

        std::optional<unsigned int> port() const;
        void setPort(const std::optional<unsigned int> &port = {});

        std::optional<std::string_view> query() const;
        void setQuery(const std::optional<std::string> &value = {});

        std::optional<std::string_view> scheme() const;
        void setScheme(const std::optional<std::string> &value = {});

#ifndef _WIN32
        std::optional<std::string_view> unixSocket() const;
        void setUnixSocket(const std::optional<std::string> &value = {});
#endif

        std::optional<std::string_view> userInfo() const;
        void setUserInfo(const std::optional<std::string> &value = {});

        void setFlags(unsigned int flags);

        std::string join() const;

    private:
        PossiblyUnownedPointer<evhttp_uri, EvHTTPURIDeleter> m_uri;
    };
}

#endif

#ifndef LLSERVICES_NETWORKING_EVENT_TYPES_H
#define LLSERVICES_NETWORKING_EVENT_TYPES_H

#include "event2/buffer.h"
#include <memory>

#include <event2/event.h>
#include <event2/listener.h>
#include <event2/watch.h>
#include <event2/dns.h>
#include <event2/http.h>
#include <event2/keyvalq_struct.h>
#include <event2/bufferevent.h>
#include <event2/ws.h>

extern "C" {
    void evws_connection_free(struct evws_connection *evws);
}

namespace LLServices {

    struct EventBaseDeleter {
        inline void operator()(struct event_base *base) {
            event_base_free(base);
        }
    };

    using EventBasePtr = std::unique_ptr<struct event_base, EventBaseDeleter>;

    struct EventConfigDeleter {
        inline void operator()(struct event_config *config) {
            event_config_free(config);
        }
    };

    using EventConfigPtr = std::unique_ptr<struct event_config, EventConfigDeleter>;

    struct EventDeleter {
        inline void operator()(struct event *ev) {
            event_free(ev);
        }
    };

    using EventPtr = std::unique_ptr<struct event, EventDeleter>;

    struct ListenerDeleter {
        inline void operator()(struct evconnlistener *ptr) const {
            evconnlistener_free(ptr);
        }
    };

    using ListenerPtr = std::unique_ptr<struct evconnlistener, ListenerDeleter>;

    struct WatchDeleter {
        inline void operator()(struct evwatch *ptr) const {
            evwatch_free(ptr);
        }
    };

    using WatchPtr = std::unique_ptr<struct evwatch, WatchDeleter>;

    struct EvDNSBaseDeleter {
        inline void operator()(struct evdns_base *ptr) const {
            evdns_base_free(ptr, 1);
        }
    };

    using EvDNSBasePtr = std::unique_ptr<struct evdns_base, EvDNSBaseDeleter>;

    struct EvHTTPURIDeleter {
        inline void operator()(struct evhttp_uri *ptr) const {
            evhttp_uri_free(ptr);
        }
    };

    using EvHTTPURI = std::unique_ptr<struct evhttp_uri, EvHTTPURIDeleter>;

    struct BufferEventDeleter {
        inline void operator()(struct bufferevent *ptr) const {
            bufferevent_free(ptr);
        }
    };

    using BufferEventPtr = std::unique_ptr<struct bufferevent, BufferEventDeleter>;

    struct EvHTTPConnectionDeleter {
        inline void operator()(struct evhttp_connection *ptr) const {
            evhttp_connection_free(ptr);
        }
    };

    using EvHTTPConnectionPtr = std::unique_ptr<struct evhttp_connection, EvHTTPConnectionDeleter>;

    struct EvHTTPRequestDeleter {
        inline void operator()(struct evhttp_request *req) const {
            evhttp_request_free(req);
        }
    };

    using EvHTTPRequestPtr = std::unique_ptr<struct evhttp_request, EvHTTPRequestDeleter>;

    struct EvHTTPDeleter {
        inline void operator()(struct evhttp *ptr) const {
            evhttp_free(ptr);
        }
    };

    using EvHTTPPtr = std::unique_ptr<struct evhttp, EvHTTPDeleter>;

    struct EvWSConnectionDeleter {
        inline void operator()(struct evws_connection *ptr) const {
            evws_connection_free(ptr);
        }
    };

    using EvWSConnectionPtr = std::unique_ptr<struct evws_connection, EvWSConnectionDeleter>;

    struct EvKeyValqDeleter {
        inline void operator()(struct evkeyvalq *ptr) const {
            evhttp_clear_headers(ptr);
            delete ptr;
        }
    };

    using EvKeyValqPtr = std::unique_ptr<struct evkeyvalq, EvKeyValqDeleter>;

    struct BufferDeleter {
        inline void operator()(struct evbuffer *ptr) const {
            evbuffer_free(ptr);
        }
    };

    using BufferPtr = std::unique_ptr<struct evbuffer, BufferDeleter>;
}

#endif

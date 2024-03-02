#include "LLServices/Networking/HttpRequest.h"
#include "LLServices/Networking/URI.h"
#include "LLServices/Networking/KeyValuePairs.h"
#include "LLServices/Networking/Buffer.h"

#include "event2/http.h"

#include <stdexcept>

namespace LLServices {

    HttpRequest::HttpRequest() noexcept : m_handle(nullptr) {

    }

    HttpRequest::HttpRequest(evhttp_request *request) : m_handle(request) {

    }

    HttpRequest::~HttpRequest() {
        if(m_handle && evhttp_request_is_owned(m_handle)) {
            evhttp_request_free(m_handle);
        }
    }

    void HttpRequest::keepAfterCompletion() {
        if(!m_handle)
            throw std::logic_error("no managed request");

        if(!evhttp_request_is_owned(m_handle))
            evhttp_request_own(m_handle);
    }


    HttpRequest::HttpRequest(HttpRequest &&other) noexcept : HttpRequest() {
        *this = std::move(other);
    }

    HttpRequest &HttpRequest::operator =(HttpRequest &&other) noexcept {
        if(this != &other) {

            if(m_handle && evhttp_request_is_owned(m_handle)) {
                evhttp_request_free(m_handle);
            }

            m_handle = other.m_handle;
            other.m_handle = nullptr;

        }

        return *this;
    }

    URI HttpRequest::uri() const {
        return URI(const_cast<evhttp_uri *>(evhttp_request_get_evhttp_uri(m_handle)));
    }

    KeyValuePairs HttpRequest::inputHeaders() const {
        return KeyValuePairs(evhttp_request_get_input_headers(m_handle));
    }

    KeyValuePairs HttpRequest::outputHeaders() const {
        return KeyValuePairs(evhttp_request_get_output_headers(m_handle));
    }


    void HttpRequest::sendError(int error, const char *reason) {
        bool owned = evhttp_request_is_owned(m_handle);

        evhttp_send_error(m_handle, error, reason);

        if(!owned)
            m_handle = nullptr;
    }

    void HttpRequest::sendReply(int error, const char *reason, Buffer &response) {
        bool owned = evhttp_request_is_owned(m_handle);

        evhttp_send_reply(m_handle, error, reason, response.get());

        if(!owned)
            m_handle = nullptr;
    }

    void HttpRequest::sendReplyStartChunked(int error, const char *reason) {
        evhttp_send_reply_start(m_handle, error, reason);
    }

    void HttpRequest::sendReplyChunk(Buffer &chunk) {
        evhttp_send_reply_chunk(m_handle, chunk.get());
    }

    void HttpRequest::sendReplyEnd() {
        bool owned = evhttp_request_is_owned(m_handle);

        evhttp_send_reply_end(m_handle);

        if(!owned)
            m_handle = nullptr;
    }
}

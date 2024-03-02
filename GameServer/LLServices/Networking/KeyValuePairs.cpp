#include "LLServices/Networking/KeyValuePairs.h"
#include "event2/http.h"

namespace LLServices {

    KeyValuePairs::KeyValuePairs(evkeyvalq *object) : m_object(object) {

    }

    KeyValuePairs::KeyValuePairs(EvKeyValqPtr &&object) : m_object(std::move(object)) {

    }


    KeyValuePairs::KeyValuePairs(KeyValuePairs &&other) noexcept = default;

    KeyValuePairs &KeyValuePairs::operator =(KeyValuePairs &&other) noexcept = default;

    void KeyValuePairs::clear() {
        evhttp_clear_headers(m_object.get());
    }

    void KeyValuePairs::add(const char *key, const char *value) {
        if(evhttp_add_header(m_object.get(), key, value) < 0)
            throw std::runtime_error("evhttp_add_header has failed");
    }

    const char *KeyValuePairs::find(const char *key) const {
        return evhttp_find_header(m_object.get(), key);
    }

    bool KeyValuePairs::remove(const char *key) {
        return evhttp_remove_header(m_object.get(), key) >= 0;
    }
}


#ifndef LLSERVICES_NETWORKING_KEY_VALUE_PAIRS_H
#define LLSERVICES_NETWORKING_KEY_VALUE_PAIRS_H

#include "LLServices/Networking/EventTypes.h"
#include "LLServices/Networking/PossiblyUnownedPointer.h"

struct evkeyvalq;

namespace LLServices {

    class KeyValuePairs {
    public:
        /*
         * Creates KeyValuePairs over an *unowned* evkeyvalq.
         */
        explicit KeyValuePairs(evkeyvalq *object);

        /*
         * Creates KeyValuePairs over an *owned* evkeyvalq.
         */
        explicit KeyValuePairs(EvKeyValqPtr &&object);

        KeyValuePairs(const KeyValuePairs &other) = delete;
        KeyValuePairs &operator =(const KeyValuePairs &other) = delete;

        KeyValuePairs(KeyValuePairs &&other) noexcept;
        KeyValuePairs &operator =(KeyValuePairs &&other) noexcept;

        void clear();

        void add(const char *key, const char *value);
        bool remove(const char *key);

        const char *find(const char *key) const;


    private:
        PossiblyUnownedPointer<evkeyvalq, EvKeyValqDeleter> m_object;
    };
}

#endif

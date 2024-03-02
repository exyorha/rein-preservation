#ifndef CALL_H
#define CALL_H

#include "MetadataArray.h"
#include <string>

namespace minigrpc {

    class Channel;
    class CompletionQueue;
    class BatchContext;
    class SliceBuffer;

    class Call {
    public:
        Call(
            const Channel *channel,
            CompletionQueue *cq,
            const char *method
        );
        ~Call();

        Call(const Call &other) = delete;
        Call &operator =(const Call &other) = delete;

        void start(
            BatchContext *completionContext,
            const SliceBuffer *outgoingData,
            const MetadataArray *metadata
        );

    private:
        CompletionQueue *m_cq;
        std::string m_url;
    };

}

#endif

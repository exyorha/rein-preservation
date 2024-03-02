#include "Call.h"
#include "Channel.h"
#include "SliceBuffer.h"
#include "CallExecutor.h"
#include "grpcsharp.h"
#include "ExecutingCall.h"

#include <sstream>

namespace minigrpc {

    Call::Call(
        const Channel *channel,
        CompletionQueue *cq,
        const char *method
    ) : m_cq(cq) {

        std::stringstream urlStream;
        urlStream << "http://" << channel->target() << method;
        m_url = urlStream.str();
    }

    Call::~Call() = default;

    void Call::start(
        BatchContext *completionContext,
        const SliceBuffer *outgoingData,
        const MetadataArray *metadata
    ) {

        std::vector<unsigned char> outgoingDataBytes;
        if(outgoingData) {
            outgoingDataBytes = outgoingData->bytes();
        }

        std::vector<std::pair<std::string, std::string>> metadataPairs;
        if(metadata) {
            metadataPairs = metadata->metadata();
        }

        auto call = std::make_unique<ExecutingCall>(
            m_url,
            m_cq,
            completionContext,
            std::move(outgoingDataBytes),
            std::move(metadataPairs)
        );

        callExecutor.add(std::move(call));
    }
}

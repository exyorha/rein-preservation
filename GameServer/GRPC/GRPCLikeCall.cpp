#include "GRPC/GRPCLikeCall.h"
#include "GRPC/PBInputOverLLBuffer.h"

#include "GRPC/PBOutputOverLLBuffer.h"
#include "LLServices/Networking/KeyValuePairs.h"
#include "LLServices/Networking/Buffer.h"

#include <google/protobuf/message.h>

GRPCLikeCall::GRPCLikeCall(LLServices::HttpRequest &&request, google::protobuf::Service *service, const google::protobuf::MethodDescriptor *method) :
    m_log(LLServices::LogFacility::instanceName("GRPCLikeCall", this)),
    m_request(std::move(request)),
    m_service(service),
    m_method(method) {

    m_requestMessage.reset(m_service->GetRequestPrototype(m_method).New());
    m_responseMessage.reset(m_service->GetResponsePrototype(m_method).New());

    PBInputOverLLBuffer input(m_request.inputBuffer());
    if(!m_requestMessage->ParseFromZeroCopyStream(&input))
        throw std::runtime_error("failed to parse the request");
}

GRPCLikeCall::~GRPCLikeCall() = default;

void GRPCLikeCall::Reset() {
    m_callError.reset();
}

bool GRPCLikeCall::Failed() const {
    return m_callError.has_value();
}

std::string GRPCLikeCall::ErrorText() const {
    return m_callError.value();
}

void GRPCLikeCall::StartCancel() {

}

void GRPCLikeCall::SetFailed(const std::string& reason) {
    m_callError.emplace(reason);
}

bool GRPCLikeCall::IsCanceled() const {
    return false;
}

void GRPCLikeCall::NotifyOnCancel(Closure* callback) {
    (void)callback;
}

void GRPCLikeCall::execute() noexcept {
    try {

        m_service->CallMethod(m_method, this, m_requestMessage.get(), m_responseMessage.get(), this);

    } catch(const std::exception &e) {
        finishWithException(e);
    }
}

void GRPCLikeCall::finishWithException(const std::exception &e) {
    m_log.error("failed: %s\n", e.what());

    m_request.outputHeaders().add("Content-Type", "text/plain");

    LLServices::Buffer buf;
    buf.append(e.what());

    m_request.sendReply(400, "Bad Request", buf);

    delete this;
}

void GRPCLikeCall::Run() {
    try {
        if(m_callError.has_value()) {
            throw std::runtime_error(*m_callError);

        } else {
            LLServices::Buffer response;

            PBOutputOverLLBuffer output(response.get());
            if(!m_responseMessage->SerializeToZeroCopyStream(&output))
                throw std::runtime_error("failed to serialize the response");

            m_request.outputHeaders().add("Content-Type", "application/octet-stream");

            m_request.sendReply(200, "OK", response);
        }
    } catch(const std::exception &e) {
        finishWithException(e);
    }
}

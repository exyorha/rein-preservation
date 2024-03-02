#ifndef GRPC_GRPC_LIKE_CALL_H
#define GRPC_GRPC_LIKE_CALL_H

#include "LLServices/Logging/LogFacility.h"
#include "LLServices/Networking/HttpRequest.h"

#include <google/protobuf/service.h>

class GRPCLikeCall final : public google::protobuf::RpcController, private google::protobuf::Closure {
public:
    GRPCLikeCall(LLServices::HttpRequest &&request, google::protobuf::Service *service, const google::protobuf::MethodDescriptor *method);

private:
    ~GRPCLikeCall() override;

public:
    GRPCLikeCall(const GRPCLikeCall &other) = delete;
    GRPCLikeCall &operator =(const GRPCLikeCall &other) = delete;

    void execute() noexcept;

    void Reset() override;
    bool Failed() const override;
    std::string ErrorText() const override;
    void StartCancel() override;
    void SetFailed(const std::string& reason) override;
    bool IsCanceled() const override;
    void NotifyOnCancel(Closure* callback) override;

    inline LLServices::HttpRequest &httpRequest() {
        return m_request;
    }

private:
    void Run() override;

    void finishWithException(const std::exception &e);

    LLServices::LogFacility m_log;
    LLServices::HttpRequest m_request;
    google::protobuf::Service *m_service;
    const google::protobuf::MethodDescriptor *m_method;
    std::unique_ptr<google::protobuf::Message> m_requestMessage;
    std::unique_ptr<google::protobuf::Message> m_responseMessage;
    std::optional<std::string> m_callError;
};



#endif

#include "GRPC/GRPCLikeServer.h"
#include "GRPC/GRPCLikeCall.h"

#include "LLServices/Logging/LogFacility.h"
#include "LLServices/Networking/HttpRequest.h"
#include "LLServices/Networking/URI.h"
#include "LLServices/Networking/KeyValuePairs.h"
#include "LLServices/Networking/Buffer.h"
#include "event2/http.h"

#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>

#include <google/protobuf/service.h>

#include <sstream>

LLServices::LogFacility LogGRPCLikeServer("GRPCLikeServer");

GRPCLikeServer::GRPCLikeServer() = default;

GRPCLikeServer::~GRPCLikeServer() = default;

void GRPCLikeServer::registerService(google::protobuf::Service *service) {
    m_services.emplace_back(service);
}

void GRPCLikeServer::handle(const std::string_view &path, LLServices::HttpRequest &&request) {

    GRPCLikeCall *call;

    try {
        if(request.command() != EVHTTP_REQ_POST)
            throw std::runtime_error("POST expected");

        auto serviceNameDelimiter = path.find('/', 1);
        if(serviceNameDelimiter == std::string_view::npos)
            throw std::runtime_error("no service name delimiter");

        auto serviceName = path.substr(1, serviceNameDelimiter - 1);
        auto methodName = path.substr(serviceNameDelimiter + 1);

        auto service = findService(serviceName);
        if(!service) {
            throw std::runtime_error("no such service: " + std::string(serviceName));
        }

        auto method = service->GetDescriptor()->FindMethodByName(std::string(methodName));
        if(!method) {
            std::stringstream error;
            error << "'" << serviceName << "' has no method '" << methodName << "'";
            throw std::runtime_error("no such service: " + std::string(serviceName));
        }

        call = new GRPCLikeCall(std::move(request), service, method);

    } catch(const std::exception &e) {
        request.outputHeaders().add("Content-Type", "text/plain");

        LLServices::Buffer buf;
        buf.append(e.what());

        LogGRPCLikeServer.error("failed to start a call: %s\n", e.what());

        request.sendReply(400, "Bad Request", buf);

        return;
    }

    call->execute();
}

google::protobuf::Service *GRPCLikeServer::findService(const std::string_view &serviceName) const {

    for(auto servicePtr: m_services) {
        if(servicePtr->GetDescriptor()->full_name() == serviceName)
            return servicePtr;
    }

    return nullptr;
}

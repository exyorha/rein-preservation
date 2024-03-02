#ifndef GRPC_GRPC_LIKE_SERVER_H
#define GRPC_GRPC_LIKE_SERVER_H

#include "WebServices/WebRoutable.h"

#include <vector>
#include <string_view>

namespace LLServices {
    class LogFacility;
}

namespace google::protobuf {
    class Service;
}

class GRPCLikeServer final : public WebRoutable {
public:
    GRPCLikeServer();
    ~GRPCLikeServer();

    GRPCLikeServer(const GRPCLikeServer &other) = delete;
    GRPCLikeServer &operator =(const GRPCLikeServer &other) = delete;

    void handle(const std::string_view &routedPath, LLServices::HttpRequest &&request) override;

    void registerService(google::protobuf::Service *service);

private:
    google::protobuf::Service *findService(const std::string_view &name) const;

    std::vector<google::protobuf::Service *> m_services;
};

extern LLServices::LogFacility LogGRPCLikeServer;

#endif

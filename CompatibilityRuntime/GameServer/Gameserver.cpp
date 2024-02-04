#include "Gameserver.h"

#include <grpc/grpc.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/support/interceptor.h>

class APIEncryptionInterceptorFactory final : public grpc::experimental::ServerInterceptorFactoryInterface {
public:
    APIEncryptionInterceptorFactory();
    ~APIEncryptionInterceptorFactory() override;

    grpc::experimental::Interceptor* CreateServerInterceptor(grpc::experimental::ServerRpcInfo* info) override;
};

class APIEncryptionInterceptor final : public grpc::experimental::Interceptor {
public:
    APIEncryptionInterceptor();
    ~APIEncryptionInterceptor() override;

    void Intercept(grpc::experimental::InterceptorBatchMethods* methods) override;
};

APIEncryptionInterceptorFactory::APIEncryptionInterceptorFactory() = default;

APIEncryptionInterceptorFactory::~APIEncryptionInterceptorFactory() = default;

grpc::experimental::Interceptor* APIEncryptionInterceptorFactory::CreateServerInterceptor(grpc::experimental::ServerRpcInfo* info) {
    printf("!!! Creating an encryption interceptor for the inbound call to %s\n", info->method());

    return new APIEncryptionInterceptor();
}

APIEncryptionInterceptor::APIEncryptionInterceptor() = default;

APIEncryptionInterceptor::~APIEncryptionInterceptor() = default;

void APIEncryptionInterceptor::Intercept(grpc::experimental::InterceptorBatchMethods* methods) {
    printf("Called to intercept!\n");

    methods->Proceed();
}

Gameserver::Gameserver()  {
    grpc::ServerBuilder grpcBuilder;
    grpcBuilder.RegisterService(&m_userService);
    grpcBuilder.RegisterService(&m_dataService);
    grpcBuilder.RegisterService(&m_gamePlayService);
    grpcBuilder.RegisterService(&m_questService);
    grpcBuilder.RegisterService(&m_gimmickService);

    std::vector<std::unique_ptr<grpc::experimental::ServerInterceptorFactoryInterface>> interceptorCreators;
    interceptorCreators.emplace_back(std::make_unique<APIEncryptionInterceptorFactory>());

    grpcBuilder.experimental().SetInterceptorCreators(std::move(interceptorCreators));

    //m_rpcThread.emplace(grpcBuilder.AddCompletionQueue(true));

    grpcBuilder.AddListeningPort("127.0.0.1:8087", grpc::InsecureServerCredentials());

    m_server = grpcBuilder.BuildAndStart();
    if(!m_server)
        throw std::runtime_error("failed to create the gRPC server");

}

Gameserver::~Gameserver() {
    m_server->Shutdown();
}

/*
 * Obviously, this is not the right way to do this. But what is the right way?
 */

grpc_channel* grpc_inproc_channel_create(grpc_server* server,
                                        const grpc_channel_args* args,
                                        void* reserved);

grpc_channel *Gameserver::openInProcessChannel(const grpc_channel_args *args) {
    return grpc_inproc_channel_create(m_server->c_server(), args, nullptr);
}

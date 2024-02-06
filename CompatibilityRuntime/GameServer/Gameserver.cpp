#include "Gameserver.h"

#include <grpc/grpc.h>
#include <grpcpp/server_builder.h>


Gameserver::Gameserver(const std::filesystem::path &individualDatabasePath, const std::filesystem::path &masterDatabasePath) :
    m_db(individualDatabasePath, masterDatabasePath) {
    grpc::ServerBuilder grpcBuilder;
    grpcBuilder.RegisterService(&m_userService);
    grpcBuilder.RegisterService(&m_dataService);
    grpcBuilder.RegisterService(&m_gamePlayService);
    grpcBuilder.RegisterService(&m_questService);
    grpcBuilder.RegisterService(&m_gimmickService);

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

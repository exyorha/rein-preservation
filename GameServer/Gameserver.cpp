#include "Gameserver.h"

#include <grpc/grpc.h>
#include <grpcpp/server_builder.h>

Gameserver::Gameserver(const std::filesystem::path &individualDatabasePath, const std::filesystem::path &masterDatabasePath) :
    m_db(individualDatabasePath, masterDatabasePath),
    m_userService(m_db),
    m_dataService(m_db),
    m_gamePlayService(m_db),
    m_questService(m_db),
    m_gimmickService(m_db),
    m_notificationService(m_db),
    m_gachaService(m_db) {

    grpc::ServerBuilder grpcBuilder;
    grpcBuilder.RegisterService(&m_userService);
    grpcBuilder.RegisterService(&m_dataService);
    grpcBuilder.RegisterService(&m_gamePlayService);
    grpcBuilder.RegisterService(&m_questService);
    grpcBuilder.RegisterService(&m_gimmickService);
    grpcBuilder.RegisterService(&m_notificationService);
    grpcBuilder.RegisterService(&m_gachaService);
    grpcBuilder.SetSyncServerOption(grpc::ServerBuilder::NUM_CQS, 1);

    grpcBuilder.AddListeningPort("127.0.0.1:8087", grpc::InsecureServerCredentials());

    m_server = grpcBuilder.BuildAndStart();
    if(!m_server)
        throw std::runtime_error("failed to create the gRPC server");

}

Gameserver::~Gameserver() {
    m_server->Shutdown();
}

void Gameserver::wait() {
    m_server->Wait();
}

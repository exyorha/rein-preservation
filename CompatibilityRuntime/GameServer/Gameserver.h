#ifndef GAMESERVER_H
#define GAMESERVER_H

#include <grpc/grpc.h>
#include <grpcpp/server.h>

#include "RPCProcessingThread.h"

#include "ServiceImplementations/UserService.h"

struct Gameserver {
public:
    Gameserver();
    ~Gameserver();

    Gameserver(const Gameserver &other) = delete;
    Gameserver &operator =(const Gameserver &other) = delete;

    grpc_channel *openInProcessChannel(const grpc_channel_args *args);

private:
    UserService m_userService;
    std::optional<RPCProcessingThread> m_rpcThread;
    std::unique_ptr<grpc::Server> m_server;
};

#endif

#include <gameserver_api.h>
#include <cstdio>
#include <exception>

#include "Gameserver.h"

Gameserver *gameserver_create(void) {
    try {

        return new Gameserver();

    } catch(const std::exception &e) {
        fprintf(stderr, "gameserver_create: failed to initialize a game server: %s\n", e.what());

        return nullptr;
    } catch(...) {
        fprintf(stderr, "gameserver_create: failed to initialize a game server: %s\n", "no message available");

        return nullptr;
    }
}

void gameserver_destroy(Gameserver *instance) {
    delete instance;
}

void *gameserver_open_in_process_api_channel(Gameserver *gameserver, const void *args) {
    return gameserver->openInProcessChannel(static_cast<const grpc_channel_args *>(args));
}

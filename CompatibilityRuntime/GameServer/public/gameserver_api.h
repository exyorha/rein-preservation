#ifndef GAMESERVER_API_H
#define GAMESERVER_API_H

#include <gameserver_export.h>

struct Gameserver;

#if defined(__cplusplus)
extern "C" {
#endif

GAMESERVER_EXPORT Gameserver *gameserver_create(void);
GAMESERVER_EXPORT void gameserver_destroy(Gameserver *gameserver);
GAMESERVER_EXPORT void *gameserver_open_in_process_api_channel(Gameserver *gameserver, const void *args);

#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)

#include <memory>

struct GameserverDeleter {
    inline void operator()(Gameserver *gameserver) {
        gameserver_destroy(gameserver);
    }
};
using GameserverPtr = std::unique_ptr<Gameserver, GameserverDeleter>;

static inline GameserverPtr makeGameServer() {
    return GameserverPtr(gameserver_create());
}
#endif

#endif

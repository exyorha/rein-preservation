#ifndef GAME_SERVER_INTERFACE_H
#define GAME_SERVER_INTERFACE_H

#include <string>

const std::string &getGameServerEndpoint();

void setGameServerEndpoint(std::string &&endpoint);

#endif

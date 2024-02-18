#ifndef GAMESERVER_H
#define GAMESERVER_H

#include <grpc/grpc.h>
#include <grpcpp/server.h>

#include "ServiceImplementations/GachaService.h"
#include "ServiceImplementations/UserService.h"
#include "ServiceImplementations/DataService.h"
#include "ServiceImplementations/GamePlayService.h"
#include "ServiceImplementations/QuestService.h"
#include "ServiceImplementations/GimmickService.h"
#include "ServiceImplementations/NotificationService.h"
#include "ServiceImplementations/TutorialService.h"
#include "ServiceImplementations/BannerService.h"
#include "ServiceImplementations/BattleService.h"
#include "ServiceImplementations/DeckService.h"
#include "ServiceImplementations/LoginBonusService.h"

#include "DataModel/Database.h"

struct Gameserver {
public:
    Gameserver(const std::filesystem::path &individualDatabasePath, const std::filesystem::path &masterDatabasePath);
    ~Gameserver();

    Gameserver(const Gameserver &other) = delete;
    Gameserver &operator =(const Gameserver &other) = delete;

    void wait();

private:
    Database m_db;
    UserService m_userService;
    DataService m_dataService;
    GamePlayService m_gamePlayService;
    QuestService m_questService;
    GimmickService m_gimmickService;
    NotificationService m_notificationService;
    GachaService m_gachaService;
    TutorialService m_tutorialService;
    BannerService m_bannerService;
    BattleService m_battleService;
    DeckService m_deckService;
    LoginBonusService m_loginBonusService;
    std::unique_ptr<grpc::Server> m_server;
};

#endif

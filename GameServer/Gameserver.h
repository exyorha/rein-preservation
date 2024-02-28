#ifndef GAMESERVER_H
#define GAMESERVER_H

#include <grpc/grpc.h>
#include <grpcpp/server.h>

#include "ServiceImplementations/CageOrnamentService.h"
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
#include "ServiceImplementations/PortalCageService.h"
#include "ServiceImplementations/CharacterViewerService.h"
#include "ServiceImplementations/OmikujiService.h"
#include "ServiceImplementations/NaviCutInService.h"
#include "ServiceImplementations/DokanService.h"
#include "ServiceImplementations/CostumeService.h"
#include "ServiceImplementations/WeaponService.h"
#include "ServiceImplementations/CageOrnamentService.h"

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
    PortalCageService m_portalCageService;
    CharacterViewerService m_characterViewerService;
    OmikujiService m_omikujiService;
    NaviCutInService m_naviCutInService;
    DokanService m_dokanService;
    CostumeService m_costumeService;
    WeaponService m_weaponService;
    CageOrnamentService m_cageOrnamentService;
    std::unique_ptr<grpc::Server> m_server;
};

#endif

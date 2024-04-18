#ifndef GAMESERVER_H
#define GAMESERVER_H

#include "LLServices/Logging/LogManager.h"
#include "LLServices/Logging/ConsoleLogSink.h"
#include "LLServices/Networking/EventLoop.h"
#include "LLServices/Networking/HttpServer.h"

#include "WebServices/WebRouter.h"
#include "WebServices/WebContentServer.h"
#include "WebServices/OctoWebServices.h"
#include "WebServices/ServerCLIService.h"

#include "GRPC/GRPCLikeServer.h"

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
#include "ServiceImplementations/CompanionService.h"
#include "ServiceImplementations/CharacterService.h"
#include "ServiceImplementations/CharacterBoardService.h"
#include "ServiceImplementations/ContentsStoryService.h"
#include "ServiceImplementations/SideStoryQuestService.h"
#include "ServiceImplementations/ExploreService.h"
#include "ServiceImplementations/GiftService.h"
#include "ServiceImplementations/ShopService.h"

#include "DataModel/Database.h"

#include "DatabaseViewer/DatabaseViewerService.h"

struct Gameserver {
public:
    Gameserver(const std::filesystem::path &individualDatabasePath, const std::filesystem::path &masterDatabasePath,
               const std::filesystem::path &octoListPath, const std::filesystem::path &webRoot);
    ~Gameserver();

    Gameserver(const Gameserver &other) = delete;
    Gameserver &operator =(const Gameserver &other) = delete;

    inline void listen(const char *address, unsigned int port) {
        return m_http.listen(address, port);
    }

    inline void acceptConnections(evutil_socket_t fd) {
        return m_http.acceptConnections(fd);
    }

    inline LLServices::EventLoop &eventLoop() {
        return m_eventLoop;
    }

    void wait();

    static std::filesystem::path defaultWebRootPath();

private:
    LLServices::ConsoleLogSink m_logSink;
    ServerCLIService m_cliService;
    LLServices::LogManagerScope m_logManagerScope;
    LLServices::EventLoop m_eventLoop;
    GRPCLikeServer m_gameAPI;
    WebContentServer m_webServer;
    OctoWebServices m_octoServices;
    Database m_db;
    DatabaseViewerService m_dbViewer;
    WebRouter m_router;
    LLServices::HttpServer m_http;

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
    CompanionService m_companionService;
    CharacterService m_characterService;
    CharacterBoardService m_characterBoardService;
    ContentsStoryService m_contentsStoryService;
    SideStoryQuestService m_sideStoryQuestService;
    ExploreService m_exploreService;
    GiftService m_giftService;
    ShopService m_shopService;
};

#endif

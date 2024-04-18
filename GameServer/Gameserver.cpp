#include "Gameserver.h"
#include "DataModel/DatabaseEnums.h"
#include <DataModel/Sqlite/Transaction.h>
#include <DataModel/Sqlite/Statement.h>

#include <filesystem>
#include <fstream>

#if defined(_WIN32)
#include <windows.h>
#endif

Gameserver::Gameserver(const std::filesystem::path &individualDatabasePath, const std::filesystem::path &masterDatabasePath,
               const std::filesystem::path &octoListPath, const std::filesystem::path &webRoot) :
    m_cliService(&m_logSink),
    m_logManagerScope(std::make_shared<LLServices::LogManager>(&m_cliService)),
    m_webServer(webRoot),
    m_octoServices(octoListPath),
    m_db(individualDatabasePath, masterDatabasePath),
    m_dbViewer(m_octoServices.revision(), m_db),
    m_router(&m_webServer),
    m_http(&m_eventLoop, &m_router),

    m_userService(m_db),
    m_dataService(m_db),
    m_gamePlayService(m_db),
    m_questService(m_db),
    m_gimmickService(m_db),
    m_notificationService(m_db),
    m_gachaService(m_db),
    m_tutorialService(m_db),
    m_bannerService(m_db),
    m_battleService(m_db),
    m_deckService(m_db),
    m_loginBonusService(m_db),
    m_portalCageService(m_db),
    m_characterViewerService(m_db),
    m_omikujiService(m_db),
    m_naviCutInService(m_db),
    m_dokanService(m_db),
    m_costumeService(m_db),
    m_weaponService(m_db),
    m_cageOrnamentService(m_db),
    m_companionService(m_db),
    m_characterService(m_db),
    m_characterBoardService(m_db),
    m_contentsStoryService(m_db),
    m_sideStoryQuestService(m_db),
    m_exploreService(m_db),
    m_giftService(m_db),
    m_shopService(m_db) {

    m_cliService.initCLI(m_db);

    m_router.handleSubpath("/api.app.nierreincarnation.com", &m_gameAPI);
    m_router.handleSubpath("/resources-api.app.nierreincarnation.com", &m_octoServices);
    m_router.handleSubpath("/server-cli", &m_cliService);
    m_router.handleSubpath("/database", &m_dbViewer);

    std::string path("/web.app.nierreincarnation.com/assets/release/");
    path.append(m_db.masterDatabaseVersion());
    path.append("/database.bin");

    m_webServer.overridePath(std::move(path), std::filesystem::path(masterDatabasePath));

    m_gameAPI.registerService(&m_userService);
    m_gameAPI.registerService(&m_dataService);
    m_gameAPI.registerService(&m_gamePlayService);
    m_gameAPI.registerService(&m_questService);
    m_gameAPI.registerService(&m_gimmickService);
    m_gameAPI.registerService(&m_notificationService);
    m_gameAPI.registerService(&m_gachaService);
    m_gameAPI.registerService(&m_tutorialService);
    m_gameAPI.registerService(&m_bannerService);
    m_gameAPI.registerService(&m_battleService);
    m_gameAPI.registerService(&m_deckService);
    m_gameAPI.registerService(&m_loginBonusService);
    m_gameAPI.registerService(&m_portalCageService);
    m_gameAPI.registerService(&m_characterViewerService);
    m_gameAPI.registerService(&m_omikujiService);
    m_gameAPI.registerService(&m_naviCutInService);
    m_gameAPI.registerService(&m_dokanService);
    m_gameAPI.registerService(&m_costumeService);
    m_gameAPI.registerService(&m_weaponService);
    m_gameAPI.registerService(&m_cageOrnamentService);
    m_gameAPI.registerService(&m_companionService);
    m_gameAPI.registerService(&m_characterService);
    m_gameAPI.registerService(&m_characterBoardService);
    m_gameAPI.registerService(&m_contentsStoryService);
    m_gameAPI.registerService(&m_sideStoryQuestService);
    m_gameAPI.registerService(&m_exploreService);
    m_gameAPI.registerService(&m_giftService);
    m_gameAPI.registerService(&m_shopService);

}

Gameserver::~Gameserver() = default;

void Gameserver::wait() {
    m_eventLoop.run();
}

std::filesystem::path Gameserver::defaultWebRootPath() {
#ifdef _WIN32
    std::vector<wchar_t> modulePathChars(PATH_MAX);
    DWORD outLength;
    DWORD error;

    do {
        outLength = GetModuleFileName(nullptr, modulePathChars.data(), modulePathChars.size());
        if(outLength == 0) {
            throw std::runtime_error("GetModuleFileName failed");
        }

        if(error == ERROR_INSUFFICIENT_BUFFER) {
            modulePathChars.resize(modulePathChars.size() * 2);
        }

    } while(error == ERROR_INSUFFICIENT_BUFFER);

    std::filesystem::path executablePath(modulePathChars.begin(), modulePathChars.end());
#else
    auto executablePath = std::filesystem::read_symlink("/proc/self/exe");
#endif

    return executablePath.parent_path() / "WebRoot";
}

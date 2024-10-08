#include "Gameserver.h"
#include <DataModel/Sqlite/Transaction.h>
#include <DataModel/Sqlite/Statement.h>

#include <WebContentServer/LooseFileWebContentStorage.h>
#include <WebContentServer/ZipArchiveWebContentStorage.h>

#include <filesystem>

#if defined(_WIN32)
#include <windows.h>
#endif

Gameserver::Gameserver(
    const GameServerConfiguration &config,
    const std::filesystem::path &individualDatabasePath,
    const std::filesystem::path &masterDatabasePath,
    std::filesystem::path &&octoListPath,
    std::filesystem::path &&webRoot) :
    m_config(config),
    m_cliService(&m_eventLoop, &m_logSink),
    m_logManagerScope(std::make_shared<LLServices::LogManager>(&m_cliService)),
    m_contentStorage(std::move(octoListPath)),
    m_octoServices(m_contentStorage),
    m_db(individualDatabasePath, masterDatabasePath),
    m_dbViewer(m_contentStorage, m_db, webRoot.parent_path()),
    m_webAPI(m_db, webRoot.parent_path()),
    m_webServer(createWebContentStorage(std::move(webRoot))),
    m_router(&m_webServer),
    m_http(&m_eventLoop, &m_router),

    m_userService(m_db, m_config),
    m_dataService(m_db, m_config),
    m_gamePlayService(m_db, m_config),
    m_questService(m_db, m_config),
    m_gimmickService(m_db, m_config),
    m_notificationService(m_db, m_config),
    m_gachaService(m_db, m_config),
    m_tutorialService(m_db, m_config),
    m_bannerService(m_db, m_config),
    m_battleService(m_db, m_config),
    m_deckService(m_db, m_config),
    m_loginBonusService(m_db, m_config),
    m_portalCageService(m_db, m_config),
    m_characterViewerService(m_db, m_config),
    m_omikujiService(m_db, m_config),
    m_naviCutInService(m_db, m_config),
    m_dokanService(m_db, m_config),
    m_costumeService(m_db, m_config),
    m_weaponService(m_db, m_config),
    m_cageOrnamentService(m_db, m_config),
    m_companionService(m_db, m_config),
    m_characterService(m_db, m_config),
    m_characterBoardService(m_db, m_config),
    m_contentsStoryService(m_db, m_config),
    m_sideStoryQuestService(m_db, m_config),
    m_exploreService(m_db, m_config),
    m_giftService(m_db, m_config),
    m_shopService(m_db, m_config),
    m_labyrinthService(m_db, m_config),
    m_consumableItemService(m_db, m_config),
    m_materialService(m_db, m_config),
    m_friendService(m_db, m_config) {

    m_cliService.initCLI(m_db, m_config);

    m_router.handleSubpath("/web.app.nierreincarnation.com", &m_webRedirector);
    m_router.handleSubpath("/api.app.nierreincarnation.com", &m_gameAPI);
    m_router.handleSubpath("/api-web.app.nierreincarnation.com", &m_webAPI);
    m_router.handleSubpath("/resources-api.app.nierreincarnation.com", &m_octoServices);
    m_router.handleSubpath("/server-cli", &m_cliService);
    m_router.handleSubpath("/database", &m_dbViewer);

    std::string path("/assets/release/");
    path.append(m_db.masterDatabaseVersion());
    path.append("/database.bin");

    m_webServer.overridePath(std::move(path), std::filesystem::path(masterDatabasePath));

    m_webServer.setFallbackPage("/default_page.html");

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
    m_gameAPI.registerService(&m_labyrinthService);
    m_gameAPI.registerService(&m_consumableItemService);
    m_gameAPI.registerService(&m_materialService);
    m_gameAPI.registerService(&m_friendService);
}

Gameserver::~Gameserver() = default;

void Gameserver::wait() {
    m_eventLoop.run();
}

std::filesystem::path Gameserver::executableDirectory() {
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

    return executablePath.parent_path();
}

std::filesystem::path Gameserver::defaultWebRootPath() {
    auto directory = executableDirectory();

    auto looseDir = directory / "WebRoot";
    if(std::filesystem::exists(looseDir)) {
        return looseDir;
    } else {
        return directory / "WebRoot.zip";
    }
}

std::filesystem::path Gameserver::defaultConfigurationPath() {
    auto directory = executableDirectory();

    return directory / "GameServer.conf";
}

std::unique_ptr<WebContentStorage> Gameserver::createWebContentStorage(std::filesystem::path &&path) {
    if(std::filesystem::is_directory(path))
        return std::make_unique<LooseFileWebContentStorage>(std::move(path));
    else
        return std::make_unique<ZipArchiveWebContentStorage>(std::move(path));
}

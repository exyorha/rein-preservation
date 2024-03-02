#include "Gameserver.h"
#include "DataModel/DatabaseEnums.h"
#include <DataModel/Sqlite/Transaction.h>
#include <DataModel/Sqlite/Statement.h>

#include <fstream>

Gameserver::Gameserver(const std::filesystem::path &individualDatabasePath, const std::filesystem::path &masterDatabasePath) :
    m_logManagerScope(std::make_shared<LLServices::LogManager>(&m_logSink)),
    m_http(&m_eventLoop, &m_gameAPI),

    m_db(individualDatabasePath, masterDatabasePath),
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
    m_companionService(m_db) {

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

#if 0
    std::ifstream stream;
    stream.open("after_first_season.sql", std::ios::in | std::ios::binary);
    stream.seekg(0, std::ios::end);
    std::vector<char> data(stream.tellg());
    stream.seekg(0);
    stream.read(data.data(), data.size());
    {

        sqlite::Transaction transaction(&m_db.db());
        const char * ptr = data.data();
        while(*ptr) {
            auto statement = m_db.db().prepare(ptr, 0, &ptr);
            if(!statement)
                break;

            statement->exec();
        }
        transaction.commit();
    }
#endif
#if 0
    {
        sqlite::Transaction transaction(&m_db.db());

        m_db.db().prepare("UPDATE i_user_main_quest_main_flow_status SET current_quest_scene_id = 43")->exec();
        transaction.commit();
    }
#endif
#if 0
    {
        sqlite::Transaction transaction(&m_db.db());

        UserContext ctx(m_db, 1);
        //ctx.givePossession(static_cast<int32_t>(PossessionType::COSTUME), 34012, 1);
        //ctx.givePossession(static_cast<int32_t>(PossessionType::WEAPON), 350641, 1);
        //ctx.givePossession(static_cast<int32_t>(PossessionType::COSTUME), 32025, 1);
        //ctx.givePossession(static_cast<int32_t>(PossessionType::WEAPON),320461, 1);
        //ctx.givePossession(static_cast<int32_t>(PossessionType::COSTUME), 34043, 1);
        //ctx.givePossession(static_cast<int32_t>(PossessionType::WEAPON),340761, 1);
        ctx.givePossession(static_cast<int32_t>(PossessionType::WEAPON),350011,1);
        ctx.givePossession(static_cast<int32_t>(PossessionType::WEAPON),350531,1);

    ctx.givePossession(static_cast<int32_t>(PossessionType::WEAPON),340851,1);
    ctx.givePossession(static_cast<int32_t>(PossessionType::WEAPON),340741,1);

    ctx.givePossession(static_cast<int32_t>(PossessionType::WEAPON),320641,1);
    ctx.givePossession(static_cast<int32_t>(PossessionType::WEAPON),320561,1);
        transaction.commit();
    }
#endif
}

Gameserver::~Gameserver() = default;

void Gameserver::wait() {
    m_eventLoop.run();
}

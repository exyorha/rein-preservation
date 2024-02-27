#include "Gameserver.h"
#include "DataModel/DatabaseEnums.h"
#include <DataModel/Sqlite/Transaction.h>
#include <DataModel/Sqlite/Statement.h>

#include <grpc/grpc.h>
#include <grpcpp/server_builder.h>
#include <fstream>

Gameserver::Gameserver(const std::filesystem::path &individualDatabasePath, const std::filesystem::path &masterDatabasePath) :
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
    m_weaponService(m_db) {

    grpc::ServerBuilder grpcBuilder;
    grpcBuilder.RegisterService(&m_userService);
    grpcBuilder.RegisterService(&m_dataService);
    grpcBuilder.RegisterService(&m_gamePlayService);
    grpcBuilder.RegisterService(&m_questService);
    grpcBuilder.RegisterService(&m_gimmickService);
    grpcBuilder.RegisterService(&m_notificationService);
    grpcBuilder.RegisterService(&m_gachaService);
    grpcBuilder.RegisterService(&m_tutorialService);
    grpcBuilder.RegisterService(&m_bannerService);
    grpcBuilder.RegisterService(&m_battleService);
    grpcBuilder.RegisterService(&m_deckService);
    grpcBuilder.RegisterService(&m_loginBonusService);
    grpcBuilder.RegisterService(&m_portalCageService);
    grpcBuilder.RegisterService(&m_characterViewerService);
    grpcBuilder.RegisterService(&m_omikujiService);
    grpcBuilder.RegisterService(&m_naviCutInService);
    grpcBuilder.RegisterService(&m_dokanService);
    grpcBuilder.RegisterService(&m_costumeService);
    grpcBuilder.RegisterService(&m_weaponService);
    grpcBuilder.SetSyncServerOption(grpc::ServerBuilder::NUM_CQS, 1);

    grpcBuilder.AddListeningPort("0.0.0.0:8087", grpc::InsecureServerCredentials());

    m_server = grpcBuilder.BuildAndStart();
    if(!m_server)
        throw std::runtime_error("failed to create the gRPC server");
#if 0
    std::ifstream stream;
    stream.open("individual.sql", std::ios::in | std::ios::binary);
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
        ctx.givePossession(static_cast<int32_t>(PossessionType::COSTUME), 34043, 1);
        ctx.givePossession(static_cast<int32_t>(PossessionType::WEAPON),340761, 1);

        transaction.commit();
    }
#endif
}

Gameserver::~Gameserver() {
    m_server->Shutdown();
}

void Gameserver::wait() {
    m_server->Wait();
}

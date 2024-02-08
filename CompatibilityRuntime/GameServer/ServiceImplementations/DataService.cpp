#include "DataService.h"

#include <DataModel/Sqlite/Statement.h>
#include <DataModel/DatabaseJSONRepresentation.h>

#include "JSONWriter.h"

DataService::DataService(Database &db) : CommonService(db) {

}

DataService::~DataService() = default;

::grpc::Status DataService::GetLatestMasterDataVersion(::grpc::ServerContext* context,
                                                       const ::google::protobuf::Empty* request,
                                                       ::apb::api::data::MasterDataGetLatestVersionResponse* response) {

    return inCall("DataService::GetLatestMasterDataVersion", context, request, response, &DataService::GetLatestMasterDataVersionImpl);
}


void DataService::GetLatestMasterDataVersionImpl(const ::google::protobuf::Empty* request,
                                                 ::apb::api::data::MasterDataGetLatestVersionResponse* response) {

    response->set_latest_master_data_version(dataModel().masterDatabaseVersion());
}

::grpc::Status DataService::GetUserDataName(::grpc::ServerContext* context, const ::google::protobuf::Empty* request,
                                            ::apb::api::data::UserDataGetNameResponse* response) {

    return inCall("DataService::GetUserDataName", context, request, response, &DataService::GetUserDataNameImpl);
}

void DataService::GetUserDataNameImpl(const ::google::protobuf::Empty* request, ::apb::api::data::UserDataGetNameResponse* response) {



    for(const auto &name: getUserDataName()) {
        response->add_table_name(name);
    }
}


::grpc::Status DataService::GetUserDataNameV2(::grpc::ServerContext* context, const ::google::protobuf::Empty* request,
                                              ::apb::api::data::UserDataGetNameResponseV2* response) {

    return inCall("DataService::GetUserDataNameV2", context, request, response, &DataService::GetUserDataNameV2Impl);
}

void DataService::GetUserDataNameV2Impl(const ::google::protobuf::Empty* request,
                                        ::apb::api::data::UserDataGetNameResponseV2* response) {

    /*
     * The live server supplies *two* table name lists. The first list
     * contains all tables but few tables contained on the second list,
     * and the second list contains the following tables:
     *
     * "IUserQuest",
     * "IUserQuestMission",
     * "IUserMission".
     *
     * The meaning of this is unknown.
     */

    auto list = response->add_table_name_list();

    for(const auto &name: getUserDataName()) {
        list->add_table_name(name);
    }
}

std::vector<std::string> DataService::getUserDataName() const {
    std::vector<std::string> names;

    auto statement = db().prepare("SELECT name FROM pragma_table_list WHERE schema = 'main' AND type = 'table' AND name LIKE 'i_%'");
    while(statement->step()) {
        names.emplace_back(tableNameToEntityName(statement->columnText(0)));
    }

    return names;
}

::grpc::Status DataService::GetUserData(::grpc::ServerContext* context,
                                        const ::apb::api::data::UserDataGetRequest* request,
                                        ::apb::api::data::UserDataGetResponse* response) {

    return inAuthenticatedCall("DataService::GetUserData", context, request, response, &DataService::GetUserDataImpl);
}


void DataService::GetUserDataImpl(int64_t userId,
                                  const ::apb::api::data::UserDataGetRequest* request,
                                  ::apb::api::data::UserDataGetResponse* response) {

    JSONWriter json;

    for(const auto &tableName: request->table_name()) {
        json.writeArrayOpen();

        std::stringstream query;
        query << "SELECT * FROM " << entityNameToTableNameChecked(tableName) << " WHERE user_id = ?";
        auto statement = db().prepare(query.str());
        statement->bind(1, userId);

        auto colCount = statement->columnCount();

        std::optional<std::vector<std::string>> columnNames;

        while(statement->step()) {
            json.writeMapOpen();

            if(!columnNames.has_value()) {
                columnNames.emplace();

                columnNames->reserve(colCount);

                for(int colIndex = 0; colIndex < colCount; colIndex++) {
                    auto columnName = statement->columnName(colIndex);

                    columnNames->emplace_back(columnNameToEntityFieldName(columnName));
                }
            }

            for(int colIndex = 0; colIndex < colCount; colIndex++) {
                const auto &columnName = (*columnNames)[colIndex];

                json.writeString(columnName);

                writeSQLiteColumnValue(json, *statement, colIndex);
            }

            json.writeMapClose();
        }

        json.writeArrayClose();
#if 0
        std::string_view content;

        if(tableName == "IUserProfile") {
            content = R"json([{"userId":1,"name":"Ash","nameUpdateDatetime":28800000,"message":"","messageUpdateDatetime":28800000,"favoriteCostumeId":0,"favoriteCostumeIdUpdateDatetime":28800000,"latestVersion":1}])json";
        } else if(tableName == "IUser") {
            content = R"json([{"userId":1,"playerId":259591998318,"osType":2,"platformType":2,"userRestrictionType":1,"registerDatetime":1706304564000,"gameStartDatetime":28800000,"latestVersion":1}])json";
        } else if(tableName == "IUserLogin") {
            content = R"json([{"userId":1,"totalLoginCount":0,"continualLoginCount":0,"maxContinualLoginCount":0,"lastLoginDatetime":28800000,"lastComebackLoginDatetime":28800000,"latestVersion":1}]")json";
        } else if(tableName == "IUserStatus") {
            content = R"json([{"userId":1,"level":1,"exp":0,"staminaMilliValue":50000,"staminaUpdateDatetime":1706304564000,"latestVersion":1}])json";
        } else if(tableName == "IUserGem") {
            content = R"json([{"userId":1,"paidGem":0,"freeGem":0}])json";
        } else if(tableName == "IUserMainQuestMainFlowStatus") {
            content = R"json([{"userId":1,"currentMainQuestRouteId":1,"currentQuestSceneId":0,"headQuestSceneId":0,"isReachedLastQuestScene":false,"latestVersion":1}])json";
        } else {
            content = "[]";
        }
#endif

        response->mutable_user_data_json()->emplace(tableName, json.output());

        json.clear();
        json.reset();
    }
}


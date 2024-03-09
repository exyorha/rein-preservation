#include "DataService.h"

#include <DataModel/Sqlite/Statement.h>
#include <DataModel/DatabaseJSONRepresentation.h>

#include <LLServices/JSON/JSONWriter.h>

DataService::DataService(Database &db) : CommonService(db) {

}

DataService::~DataService() = default;

void DataService::GetLatestMasterDataVersion(::google::protobuf::RpcController* controller,
                        const ::google::protobuf::Empty* request,
                        ::apb::api::data::MasterDataGetLatestVersionResponse* response,
                        ::google::protobuf::Closure* done)  {

    return inCall("DataService::GetLatestMasterDataVersion", controller, request, response, done,
                  &DataService::GetLatestMasterDataVersionImpl);
}


void DataService::GetLatestMasterDataVersionImpl(const ::google::protobuf::Empty* request,
                                                 ::apb::api::data::MasterDataGetLatestVersionResponse* response) {

    response->set_latest_master_data_version(global().dataModel().masterDatabaseVersion());
}

void DataService::GetUserDataName(::google::protobuf::RpcController* controller,
                        const ::google::protobuf::Empty* request,
                        ::apb::api::data::UserDataGetNameResponse* response,
                        ::google::protobuf::Closure* done) {

    return inCall("DataService::GetUserDataName", controller, request, response, done, &DataService::GetUserDataNameImpl);
}

void DataService::GetUserDataNameImpl(const ::google::protobuf::Empty* request, ::apb::api::data::UserDataGetNameResponse* response) {



    for(const auto &name: global().getUserDataName()) {
        response->add_table_name(name);
    }
}


void DataService::GetUserDataNameV2(::google::protobuf::RpcController* controller,
                        const ::google::protobuf::Empty* request,
                        ::apb::api::data::UserDataGetNameResponseV2* response,
                        ::google::protobuf::Closure* done) {

    return inCall("DataService::GetUserDataNameV2", controller, request, response, done, &DataService::GetUserDataNameV2Impl);
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

    for(const auto &name: global().getUserDataName()) {
        list->add_table_name(name);
    }
}

void DataService::GetUserData(::google::protobuf::RpcController* controller,
                        const ::apb::api::data::UserDataGetRequest* request,
                        ::apb::api::data::UserDataGetResponse* response,
                        ::google::protobuf::Closure* done) {

    return inAuthenticatedCall("DataService::GetUserData", controller, request, response, done, &DataService::GetUserDataImpl);
}


void DataService::GetUserDataImpl(UserContext &user,
                                  const ::apb::api::data::UserDataGetRequest* request,
                                  ::apb::api::data::UserDataGetResponse* response) {

    LLServices::JSONWriter json;

    for(const auto &tableName: request->table_name()) {
        user.serializeTable(tableName, json);

        response->mutable_user_data_json()->emplace(tableName, json.output());

        json.clear();
        json.reset();
    }
}


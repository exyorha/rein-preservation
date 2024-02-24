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

    response->set_latest_master_data_version(global().dataModel().masterDatabaseVersion());
}

::grpc::Status DataService::GetUserDataName(::grpc::ServerContext* context, const ::google::protobuf::Empty* request,
                                            ::apb::api::data::UserDataGetNameResponse* response) {

    return inCall("DataService::GetUserDataName", context, request, response, &DataService::GetUserDataNameImpl);
}

void DataService::GetUserDataNameImpl(const ::google::protobuf::Empty* request, ::apb::api::data::UserDataGetNameResponse* response) {



    for(const auto &name: global().getUserDataName()) {
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

    for(const auto &name: global().getUserDataName()) {
        list->add_table_name(name);
    }
}

::grpc::Status DataService::GetUserData(::grpc::ServerContext* context,
                                        const ::apb::api::data::UserDataGetRequest* request,
                                        ::apb::api::data::UserDataGetResponse* response) {

    return inAuthenticatedCall("DataService::GetUserData", context, request, response, &DataService::GetUserDataImpl);
}


void DataService::GetUserDataImpl(UserContext &user,
                                  const ::apb::api::data::UserDataGetRequest* request,
                                  ::apb::api::data::UserDataGetResponse* response) {

    JSONWriter json;

    for(const auto &tableName: request->table_name()) {
        user.serializeTable(tableName, json);

        response->mutable_user_data_json()->emplace(tableName, json.output());

        json.clear();
        json.reset();
    }
}


#ifndef SERVICE_IMPLEMENTATIONS_DATA_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_DATA_SERVICE_H

#include <service/DataService.grpc.pb.h>

class DataService final : public apb::api::data::DataService::Service {
public:
    DataService();
    ~DataService();

    DataService(const DataService &other) = delete;
    DataService &operator =(const DataService &other) = delete;

    ::grpc::Status GetLatestMasterDataVersion(::grpc::ServerContext* context, const ::google::protobuf::Empty* request,
                                              ::apb::api::data::MasterDataGetLatestVersionResponse* response) override;

    ::grpc::Status GetUserDataName(::grpc::ServerContext* context, const ::google::protobuf::Empty* request,
                                   ::apb::api::data::UserDataGetNameResponse* response) override;

    ::grpc::Status GetUserDataNameV2(::grpc::ServerContext* context, const ::google::protobuf::Empty* request,
                                     ::apb::api::data::UserDataGetNameResponseV2* response) override;

    ::grpc::Status GetUserData(::grpc::ServerContext* context,
                               const ::apb::api::data::UserDataGetRequest* request,
                               ::apb::api::data::UserDataGetResponse* response) override;

private:
    std::vector<std::string> getUserDataName() const;
    std::vector<std::string> getUserDataName2() const;

};

#endif


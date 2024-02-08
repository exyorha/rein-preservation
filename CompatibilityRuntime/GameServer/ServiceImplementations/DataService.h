#ifndef SERVICE_IMPLEMENTATIONS_DATA_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_DATA_SERVICE_H

#include <service/DataService.grpc.pb.h>

#include <ServiceImplementations/CommonService.h>

/*
 * The implementation is complete.
 */
class DataService final : public apb::api::data::DataService::Service, public CommonService {
public:
    explicit DataService(Database &db);
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
    void GetLatestMasterDataVersionImpl(const ::google::protobuf::Empty* request,
                                        ::apb::api::data::MasterDataGetLatestVersionResponse* response);

    void GetUserDataNameImpl(const ::google::protobuf::Empty* request,
                             ::apb::api::data::UserDataGetNameResponse* response);

    void GetUserDataNameV2Impl(const ::google::protobuf::Empty* request,
                               ::apb::api::data::UserDataGetNameResponseV2* response);

    void GetUserDataImpl(int64_t userId,
                         const ::apb::api::data::UserDataGetRequest* request,
                         ::apb::api::data::UserDataGetResponse* response);

    std::vector<std::string> getUserDataName() const;
};

#endif


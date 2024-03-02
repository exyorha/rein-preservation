#ifndef SERVICE_IMPLEMENTATIONS_DATA_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_DATA_SERVICE_H

#include <service/DataService.pb.h>

#include <ServiceImplementations/CommonService.h>

/*
 * The implementation is complete.
 */
class DataService final : public apb::api::data::DataService, public CommonService {
public:
    explicit DataService(Database &db);
    ~DataService();

    DataService(const DataService &other) = delete;
    DataService &operator =(const DataService &other) = delete;

    void GetLatestMasterDataVersion(::google::protobuf::RpcController* controller,
                        const ::google::protobuf::Empty* request,
                        ::apb::api::data::MasterDataGetLatestVersionResponse* response,
                        ::google::protobuf::Closure* done) override;

    void GetUserDataName(::google::protobuf::RpcController* controller,
                        const ::google::protobuf::Empty* request,
                        ::apb::api::data::UserDataGetNameResponse* response,
                        ::google::protobuf::Closure* done) override;

    void GetUserDataNameV2(::google::protobuf::RpcController* controller,
                        const ::google::protobuf::Empty* request,
                        ::apb::api::data::UserDataGetNameResponseV2* response,
                        ::google::protobuf::Closure* done) override;

    void GetUserData(::google::protobuf::RpcController* controller,
                        const ::apb::api::data::UserDataGetRequest* request,
                        ::apb::api::data::UserDataGetResponse* response,
                        ::google::protobuf::Closure* done) override;

private:
    void GetLatestMasterDataVersionImpl(const ::google::protobuf::Empty* request,
                                        ::apb::api::data::MasterDataGetLatestVersionResponse* response);

    void GetUserDataNameImpl(const ::google::protobuf::Empty* request,
                             ::apb::api::data::UserDataGetNameResponse* response);

    void GetUserDataNameV2Impl(const ::google::protobuf::Empty* request,
                               ::apb::api::data::UserDataGetNameResponseV2* response);

    void GetUserDataImpl(UserContext &user,
                         const ::apb::api::data::UserDataGetRequest* request,
                         ::apb::api::data::UserDataGetResponse* response);
};

#endif


#ifndef WEB_SERVICES_OCTO_WEB_SERVICES_H
#define WEB_SERVICES_OCTO_WEB_SERVICES_H

#include "LLServices/Networking/HttpRequest.h"
#include "WebServices/WebRoutable.h"

#include "Octo/Proto/Database.pb.h"

#include <regex>
#include <filesystem>

namespace LLServices {
    class LogFacility;
    class Buffer;

}

class OctoWebServices final : public WebRoutable {
public:
    explicit OctoWebServices(const std::filesystem::path &octoListPath);
    ~OctoWebServices();

    OctoWebServices(const OctoWebServices &other) = delete;
    OctoWebServices &operator =(const OctoWebServices &other) = delete;

    void handle(const std::string_view &path, LLServices::HttpRequest &&request) override;

private:
    static unsigned int parseNumber(const std::sub_match<const char *> &match);
    static unsigned int parseNumber(const std::string_view &match);

    LLServices::Buffer handleListRequest(unsigned int appId, unsigned int version, unsigned int currentListRevision);

    Octo::Proto::Database m_db;
};

extern LLServices::LogFacility LogOctoWebServices;

#endif

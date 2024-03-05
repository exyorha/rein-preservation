#include "ServerCLIService.h"
#include "ServerCLIConnection.h"

#include "LLServices/Networking/HttpRequest.h"
#include "LLServices/Networking/WebSocketConnection.h"

ServerCLIService::ServerCLIService() = default;

ServerCLIService::~ServerCLIService() = default;

void ServerCLIService::handle(const std::string_view &routedPath, LLServices::HttpRequest &&request) {
    if(routedPath != "/") {
        request.sendError(404, "Not Found");
        return;
    }

    if(request.command() != EVHTTP_REQ_GET) {
        request.sendError(405, "Method Not Allowed");
        return;
    }

    auto cliConnection = std::make_unique<ServerCLIConnection>();

    auto connection = request.startWebSocket(cliConnection.get());
    if(!connection) {
        /*
         * A failure response was sent.
         */
        return;
    }

    cliConnection->attachConnection(std::move(connection));

    /*
     * ServerCLIConnection will self-manage its lifetime now.
     */
    (void)cliConnection.release();
}

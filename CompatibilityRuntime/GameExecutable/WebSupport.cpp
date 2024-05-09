#include <stdexcept>

#include "WebSupport.h"
#include "GameServerInterface.h"

std::string redirectURL(const std::string_view &input) {
    std::string output(input);

    if(output.starts_with("jar:")) {
        output.erase(output.begin(), output.begin() + 4);

        auto bang = output.find('!');
        if(bang != std::string::npos) {
            output.erase(bang, 1);
        }
    }

    if(output.starts_with("file://")) {
        /*
         * File URLs go through as-is.
         */


        if(output.starts_with("file://") && output[7] != '/') {
            // Happens on Windows, needs an extra slash
            output.insert(output.begin() + 7, '/');
        }
    } else {

        /*
         * All other URLs get redirected to our game server:
         *  http(s)://<host>/<path> -> http://gameserver/<original host>/<original path>
         */

        const auto &gameServer = getGameServerEndpoint();

        auto endOfScheme = output.find("://");
        if(endOfScheme == std::string::npos)
            throw std::runtime_error("cannot find end of the scheme in the URL");

        output.replace(
            0,
            endOfScheme + 2, // make sure to keep one extra slash to delimit the server base URI and the host
            gameServer);

        /*
         * Remote the port.
         */
        auto endOfAuthority = output.find('/', gameServer.size() + 1);
        auto portDelimiter = output.find(':', gameServer.size() + 1);
        if(portDelimiter != std::string::npos && endOfAuthority != std::string::npos && portDelimiter < endOfAuthority) {
            output.erase(portDelimiter, endOfAuthority - portDelimiter);
        }
    }

    printf("redirectURL: request rewritten from '%.*s' to '%s'\n", static_cast<int>(input.size()), input.data(), output.c_str());

    return output;
}

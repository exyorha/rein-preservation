#include <translator_api.h>

#include <cstdio>
#include <string>
#include <stdexcept>

#include "Octo.h"
#include "Il2CppUtilities.h"

std::string ReplacementGameServer = "http://127.0.0.1:8087";

static std::string redirectURL(const std::string &input) {
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

        auto endOfScheme = output.find("://");
        if(endOfScheme == std::string::npos)
            throw std::runtime_error("cannot find end of the scheme in the URL");

        output.replace(
            0,
            endOfScheme + 2, // make sure to keep one extra slash to delimit the server base URI and the host
            ReplacementGameServer);

        /*
         * Remote the port.
         */
        auto endOfAuthority = output.find('/', ReplacementGameServer.size() + 1);
        auto portDelimiter = output.find(':', ReplacementGameServer.size() + 1);
        if(portDelimiter != std::string::npos && endOfAuthority != std::string::npos && portDelimiter < endOfAuthority) {
            output.erase(portDelimiter, endOfAuthority - portDelimiter);
        }
    }


    return output;
}

static void *UnityWebRequest_SetUrl(
    void *thisPointer, Il2CppString *url,
    void *(*original)(void *thisPointer, Il2CppString *url)) {

    auto input = stringToUtf8(url);
    auto redirected = redirectURL(input);

    printf("UnityWebRequest: request rewritten from '%s' to '%s'\n", input.c_str(), redirected.c_str());

    return original(thisPointer, stringFromUtf8(redirected));
}
INTERPOSE_ICALL("UnityEngine.Networking.UnityWebRequest::SetUrl", UnityWebRequest_SetUrl)

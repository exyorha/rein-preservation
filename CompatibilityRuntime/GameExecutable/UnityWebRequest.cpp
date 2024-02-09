#include <translator_api.h>

#include <cstdio>
#include <string>

#include "Octo.h"
#include "OctoContentStorage.h"
#include "Il2CppUtilities.h"

static void *UnityWebRequest_SetUrl(
    void *thisPointer, Il2CppString *url,
    void *(*original)(void *thisPointer, Il2CppString *url)) {

    auto conv = stringToUtf8(url);

    printf("Web Request for '%s'\n", conv.c_str());

    if(conv.starts_with("jar:")) {
        printf("Requesting a file from the game APK, repairing the path: '%s'\n", conv.c_str());

        conv.erase(conv.begin(), conv.begin() + 4);

        auto bang = conv.find('!');
        if(bang != std::string::npos) {
            conv.erase(bang, 1);
        }

        url = stringFromUtf8(conv);
    }

    return original(thisPointer, url);
}
INTERPOSE_ICALL("UnityEngine.Networking.UnityWebRequest::SetUrl", UnityWebRequest_SetUrl)

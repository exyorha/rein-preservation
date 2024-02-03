#include <translator_api.h>

#include <cstdio>
#include <string>

#include "Octo.h"
#include "OctoContentStorage.h"

static std::string stringToUtf8(Il2CppString *url) {

    auto length = il2cpp_string_length(url);
    auto chars = il2cpp_string_chars(url);

    std::string conv;
    for(size_t index = 0; index < length; index++) {
        conv.push_back(chars[index]);
    }

    return conv;
}

static Il2CppString *stringFromUtf8(const std::string_view &conv) {
    return il2cpp_string_new_len(conv.data(), conv.size());
}

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
    } else if(conv.starts_with("file:///fakeoctopath/")) {
        /*
         * See Octo_Caching_OctoBaseCaching_GetStorageFilePath
         */

        auto resolved = contentStorageInstance->locateFile(conv.substr(21));
        if(resolved.has_value()) {

            conv.erase(7);
            conv.append(*resolved);

            printf("Octo asset request redirected to %s\n", conv.c_str());

            url = stringFromUtf8(conv);
        } else {
            fprintf(stderr, "UnityWebRequest_SetUrl: could not resolve a request for the Octo file %s\n", conv.c_str());
        }
    }

    return original(thisPointer, url);
}
INTERPOSE_ICALL("UnityEngine.Networking.UnityWebRequest::SetUrl", UnityWebRequest_SetUrl)

#if 0
static int SystemInfo_GetOperatingSystemFamily(int (*original)(void)) {
    int real = original();
    int reported = 0;

    if(real != reported) {
    //    printf("SystemInfo_GetOperatingSystemFamily: changing the reported operating system family from %d to %d\n", real, reported);
    }

    return reported;
}

INTERPOSE_ICALL("UnityEngine.SystemInfo::GetOperatingSystemFamily", SystemInfo_GetOperatingSystemFamily)
#endif

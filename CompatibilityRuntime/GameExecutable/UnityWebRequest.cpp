#include <translator_api.h>

#include <cstdio>
#include <string>

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
    }

    return original(thisPointer, url);
}
INTERPOSE_ICALL("UnityEngine.Networking.UnityWebRequest::SetUrl", UnityWebRequest_SetUrl)

static int SystemInfo_GetOperatingSystemFamily(int (*original)(void)) {
    int real = original();
    int reported = 0;

    if(real != reported) {
        printf("SystemInfo_GetOperatingSystemFamily: changing the reported operating system family from %d to %d\n", real, reported);
    }

    return reported;
}

INTERPOSE_ICALL("UnityEngine.SystemInfo::GetOperatingSystemFamily", SystemInfo_GetOperatingSystemFamily)

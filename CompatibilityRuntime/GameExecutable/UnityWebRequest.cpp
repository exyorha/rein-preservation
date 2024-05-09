#include <translator_api.h>

#include "Il2CppUtilities.h"
#include "WebSupport.h"

static void *UnityWebRequest_SetUrl(
    void *thisPointer, Il2CppString *url,
    void *(*original)(void *thisPointer, Il2CppString *url)) {

    return original(thisPointer, stringFromUtf8(redirectURL(stringToUtf8(url))));
}
INTERPOSE_ICALL("UnityEngine.Networking.UnityWebRequest::SetUrl", UnityWebRequest_SetUrl)

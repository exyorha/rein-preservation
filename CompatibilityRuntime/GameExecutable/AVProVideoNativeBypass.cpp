/*
 * This implements what's in the AVLocal library, but avoiding P/Invoke and
 * C JNI layers.
 */

#include "AVProVideoNativeBypass.h"

#include <Java/AVProMobileVideo.h>

#include <translator_api.h>

#include <cstdio>

static void AVLocal_RenderEventFunc(uint32_t event) {
    printf("AVLocal_RenderEventFunc(0x%08X)\n", event);
}

static intptr_t AVLocal_GetRenderEventFunc(void *original) {
    printf("AVLocal_GetRenderEventFunc\n");

    return reinterpret_cast<intptr_t>(AVLocal_RenderEventFunc);
}

static int32_t AVLocal_GetWidth(int32_t playerIndex, void *original) {
    return AVProMobileVideo::GetWidth(playerIndex);
}

static int32_t AVLocal_GetHeight(int32_t playerIndex, void *original) {
    return AVProMobileVideo::GetHeight(playerIndex);
}

static int32_t AVLocal_GetTextureHandle(int32_t playerIndex, void *original) {
    return AVProMobileVideo::GetTextureHandle(playerIndex);
}

static int64_t AVLocal_GetDuration(int32_t playerIndex, void *original) {
    return AVProMobileVideo::GetDuration(playerIndex);
}

static int32_t AVLocal_GetLastErrorCode(int32_t playerIndex, void *original) {
    return AVProMobileVideo::GetLastErrorCode(playerIndex);
}

static int32_t AVLocal_GetFrameCount(int32_t playerIndex, void *original) {
    return AVProMobileVideo::GetFrameCount(playerIndex);
}

static float AVLocal_GetVideoDisplayRate(int32_t playerIndex, void *original) {
    return AVProMobileVideo::GetVideoDisplayRate(playerIndex);
}

static bool AVLocal_CanPlay(int32_t playerIndex, void *original) {
    return AVProMobileVideo::CanPlay(playerIndex);
}

void installAVProVideoNativeBypass() {
    translator_divert_method("Assembly-CSharp.dll::RenderHeads.Media.AVProVideo.AndroidMediaPlayer/Native::GetRenderEventFunc",
                             AVLocal_GetRenderEventFunc);

    translator_divert_method("Assembly-CSharp.dll::RenderHeads.Media.AVProVideo.AndroidMediaPlayer/Native::_GetWidth",
                             AVLocal_GetWidth);

    translator_divert_method("Assembly-CSharp.dll::RenderHeads.Media.AVProVideo.AndroidMediaPlayer/Native::_GetHeight",
                             AVLocal_GetHeight);

    translator_divert_method("Assembly-CSharp.dll::RenderHeads.Media.AVProVideo.AndroidMediaPlayer/Native::_GetTextureHandle",
                             AVLocal_GetTextureHandle);

    translator_divert_method("Assembly-CSharp.dll::RenderHeads.Media.AVProVideo.AndroidMediaPlayer/Native::_GetDuration",
                             AVLocal_GetDuration);

    translator_divert_method("Assembly-CSharp.dll::RenderHeads.Media.AVProVideo.AndroidMediaPlayer/Native::_GetLastErrorCode",
                             AVLocal_GetLastErrorCode);

    translator_divert_method("Assembly-CSharp.dll::RenderHeads.Media.AVProVideo.AndroidMediaPlayer/Native::_GetFrameCount",
                             AVLocal_GetFrameCount);

    translator_divert_method("Assembly-CSharp.dll::RenderHeads.Media.AVProVideo.AndroidMediaPlayer/Native::_GetVideoDisplayRate",
                             AVLocal_GetVideoDisplayRate);

    translator_divert_method("Assembly-CSharp.dll::RenderHeads.Media.AVProVideo.AndroidMediaPlayer/Native::_CanPlay",
                             AVLocal_CanPlay);
}

#include <Java/AVProMobileVideo.h>
#include <Java/JNIClass.h>
#include <Java/JNIString.h>
#include <Java/JNIState.h>
#include <Java/AVProVideoPlayer.h>

AVProMobileVideo::AVProMobileVideo() : JNIObject(parent("com/RenderHeads/AVProVideo/AVProMobileVideo")) {

    printf("AVProMobileVideo: created %p\n", this);
}

AVProMobileVideo::~AVProMobileVideo() {
    printf("AVProMobileVideo: destroyed %p\n", this);
}

std::shared_ptr<JNIClass> AVProMobileVideo::makeClass() {
    auto co = std::make_shared<JNIClass>("com/RenderHeads/AVProVideo/AVProMobileVideo", parent("java/lang/Object"));

    co->registerConstructor("()V", &AVProMobileVideo::create);

    co->registerMethod("GetPluginVersion", "()Ljava/lang/String;", &AVProMobileVideo::GetPluginVersion);
    co->registerMethod("SetContext", "(Ljava/lang/Object;)V", &AVProMobileVideo::SetContext);
    co->registerMethod("CreatePlayer", "(IZZIZ)Ljava/lang/Object;", &AVProMobileVideo::CreatePlayer);
    co->registerMethod("DestroyPlayer", "(I)V", &AVProMobileVideo::DestroyPlayer);
    co->registerMethod("Deinitialise", "()V", &AVProMobileVideo::Deinitialise);

    co->registerMethod("_GetWidth", "(I)I", &AVProMobileVideo::GetWidth);
    co->registerMethod("_GetHeight", "(I)I", &AVProMobileVideo::GetHeight);
    co->registerMethod("_GetTextureHandle", "(I)I", &AVProMobileVideo::GetTextureHandle);
    co->registerMethod("_GetDuration", "(I)J", &AVProMobileVideo::GetTextureHandle);
    co->registerMethod("_GetLastErrorCode", "(I)I", &AVProMobileVideo::GetLastErrorCode);
    co->registerMethod("_GetFrameCount", "(I)I", &AVProMobileVideo::GetFrameCount);
    co->registerMethod("_GetVideoDisplayRate", "(I)F", &AVProMobileVideo::GetVideoDisplayRate);
    co->registerMethod("_CanPlay", "(I)Z", &AVProMobileVideo::CanPlay);

    return co;
}

std::shared_ptr<JNIObject> AVProMobileVideo::create() {
    return std::make_shared<AVProMobileVideo>();
}

std::shared_ptr<JNIObject> AVProMobileVideo::GetPluginVersion() {
    return std::make_shared<JNIString>("1.10.1");
}

void AVProMobileVideo::SetContext(std::shared_ptr<JNIObject> androidContextObject) {
    (void)androidContextObject;
}

std::shared_ptr<JNIObject> AVProMobileVideo::CreatePlayer(int32_t playerType, bool useFastOesPath, bool showPosterFrame, int32_t arg4, bool arg5) {
    /*
     * Player type: 1 - 'AVProVideoMediaPlayer', 2 - 'AVProVideoExoPlayer', others invalid and return null.
     */

    printf("AVProMobileVideo::CreatePlayer(%d, %d, %d, %d, %d)\n", playerType, useFastOesPath, showPosterFrame, arg4, arg5);

    return std::make_shared<AVProVideoPlayer>(0); // TODO: allocate a player index
}

void AVProMobileVideo::DestroyPlayer(int32_t playerIndex) {
    printf("AVProMobileVideo::DestroyPlayer(%d) stub\n", playerIndex);
}

int32_t AVProMobileVideo::GetWidth(int32_t playerIndex) {
    printf("AVProMobileVideo::GetWidth(%d) stub\n", playerIndex);
    return 100;
}

int32_t AVProMobileVideo::GetHeight(int32_t playerIndex) {
    printf("AVProMobileVideo::GetHeight(%d) stub\n", playerIndex);
    return 100;
}

int32_t AVProMobileVideo::GetTextureHandle(int32_t playerIndex) {
    printf("AVProMobileVideo::GetTextureHandle(%d) stub\n", playerIndex);
    return 0;
}

int64_t AVProMobileVideo::GetDuration(int32_t playerIndex) {
    printf("AVProMobileVideo::GetDuration(%d) stub\n", playerIndex);
    return 100;
}

int32_t AVProMobileVideo::GetLastErrorCode(int32_t playerIndex) {
    printf("AVProMobileVideo::GetLastErrorCode(%d) stub\n", playerIndex);
    return 0;
}

int32_t AVProMobileVideo::GetFrameCount(int32_t playerIndex) {
    printf("AVProMobileVideo::GetFrameCount(%d) stub\n", playerIndex);
    return 0;
}

float AVProMobileVideo::GetVideoDisplayRate(int32_t playerIndex) {
    printf("AVProMobileVideo::GetVideoDisplayRate(%d) stub\n", playerIndex);
    return 30.0f;
}

bool AVProMobileVideo::CanPlay(int32_t playerIndex) {
    printf("AVProMobileVideo::CanPlay(%d) stub\n", playerIndex);
    return true;
}

void AVProMobileVideo::Deinitialise() {
    printf("AVProMobileVideo::Deinitialise stub\n");
}

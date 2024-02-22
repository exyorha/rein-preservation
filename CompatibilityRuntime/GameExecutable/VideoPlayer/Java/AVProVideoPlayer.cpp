#include <VideoPlayer/Java/AVProVideoPlayer.h>
#include <Java/JNIClass.h>
#include <Java/JNIState.h>

AVProVideoPlayer::AVProVideoPlayer(int32_t playerIndex) : JNIObject(parent("com/RenderHeads/AVProVideo/AVProVideoPlayer")),
    m_playerIndex(playerIndex) {

    printf("AVProVideoPlayer: created %p\n", this);
}

AVProVideoPlayer::~AVProVideoPlayer() {
    printf("AVProVideoPlayer: destroyed %p\n", this);
}

std::shared_ptr<JNIClass> AVProVideoPlayer::makeClass() {
    auto co = std::make_shared<JNIClass>("com/RenderHeads/AVProVideo/AVProVideoPlayer", parent("java/lang/Object"));

    co->registerMethod("Update", "()V", &AVProVideoPlayer::Update);
    co->registerMethod("SetHeadRotation", "(FFFF)V", &AVProVideoPlayer::SetHeadRotation);
    co->registerMethod("GetCurrentTimeMs", "()J", &AVProVideoPlayer::GetCurrentTimeMs);
    co->registerMethod("GetSourceVideoFrameRate", "()F", &AVProVideoPlayer::GetSourceVideoFrameRate);
    co->registerMethod("IsPlaying", "()Z", &AVProVideoPlayer::IsPlaying);
    co->registerMethod("IsPaused", "()Z", &AVProVideoPlayer::IsPaused);
    co->registerMethod("IsFinished", "()Z", &AVProVideoPlayer::IsFinished);
    co->registerMethod("IsSeeking", "()Z", &AVProVideoPlayer::IsSeeking);
    co->registerMethod("IsBuffering", "()Z", &AVProVideoPlayer::IsBuffering);
    co->registerMethod("IsLooping", "()Z", &AVProVideoPlayer::IsLooping);
    co->registerMethod("HasVideo", "()Z", &AVProVideoPlayer::HasVideo);
    co->registerMethod("HasAudio", "()Z", &AVProVideoPlayer::HasAudio);
    co->registerMethod("SetFocusProps", "(FF)V", &AVProVideoPlayer::SetFocusProps);
    co->registerMethod("SetFocusEnabled", "(Z)V", &AVProVideoPlayer::SetFocusEnabled);
    co->registerMethod("SetFocusRotation", "(FFFF)V", &AVProVideoPlayer::SetFocusRotation);
    co->registerMethod("GetPlayerIndex", "()I", &AVProVideoPlayer::GetPlayerIndex);
    co->registerMethod("SetPlayerOptions", "(ZZ)V", &AVProVideoPlayer::SetPlayerOptions);
    co->registerMethod("CloseVideo", "()V", &AVProVideoPlayer::CloseVideo);
    co->registerMethod("Pause", "()V", &AVProVideoPlayer::Pause);
    co->registerMethod("OpenVideoFromFile", "(Ljava/lang/String;JLjava/lang/Object;I)Z", &AVProVideoPlayer::OpenVideoFromFile);
    co->registerMethod("SetDeinitialiseFlagged", "()V", &AVProVideoPlayer::SetDeinitialiseFlagged);
    co->registerMethod("SetLooping", "(Z)V", &AVProVideoPlayer::SetLooping);
    co->registerMethod("GetWidth", "()I", &AVProVideoPlayer::GetWidth);
    co->registerMethod("GetHeight", "()I", &AVProVideoPlayer::GetHeight);
    co->registerMethod("GetTextureHandle", "()I", &AVProVideoPlayer::GetTextureHandle);
    co->registerMethod("GetDurationMs", "()L", &AVProVideoPlayer::GetDurationMs);
    co->registerMethod("GetLastErrorCode", "()I", &AVProVideoPlayer::GetLastErrorCode);
    co->registerMethod("GetFrameCount", "()I", &AVProVideoPlayer::GetFrameCount);
    co->registerMethod("GetDisplayRate", "()I", &AVProVideoPlayer::GetDisplayRate);
    co->registerMethod("CanPlay", "()Z", &AVProVideoPlayer::CanPlay);
    co->registerMethod("RendererSetup", "(I)V", &AVProVideoPlayer::RendererSetup);
    co->registerMethod("Render", "()V", &AVProVideoPlayer::Render);
    co->registerMethod("WaitForNewFrame", "()V", &AVProVideoPlayer::WaitForNewFrame);
    co->registerMethod("DeinitialiseRender", "()V", &AVProVideoPlayer::DeinitialiseRender);

    co->registerMethod("Deinitialise", "()V", &AVProVideoPlayer::Deinitialise);

    return co;
}

/*
 * This is called periodically to pump the video player's event queue.
 */
void AVProVideoPlayer::Update() {
   printf("AVProVideoPlayer::Update stub\n");
}

void AVProVideoPlayer::SetHeadRotation(float a1, float a2, float a3, float a4) {
    printf("AVProVideoPlayer::SetHeadRotation(%f, %f, %f, %f) stub\n", a1, a2, a3, a4);
}

int64_t AVProVideoPlayer::GetCurrentTimeMs() {
    printf("AVProVideoPlayer::GetCurrentTimeMs stub\n");
    return 0;
}

float AVProVideoPlayer::GetSourceVideoFrameRate() {
    printf("AVProVideoPlayer::GetSourceVideoFrameRate stub\n");
    return 0.0f;
}

bool AVProVideoPlayer::IsPlaying() {
    printf("AVProVideoPlayer::IsPlaying stub\n");
    return false;
}

bool AVProVideoPlayer::IsPaused() {
    printf("AVProVideoPlayer::IsPaused stub\n");
    return false;
}

bool AVProVideoPlayer::IsFinished() {
    printf("AVProVideoPlayer::IsFinished stub\n");
    return false;
}

bool AVProVideoPlayer::IsSeeking() {
    printf("AVProVideoPlayer::IsSeeking stub\n");
    return false;
}

bool AVProVideoPlayer::IsBuffering() {
    printf("AVProVideoPlayer::IsBuffering stub\n");
    return false;
}

bool AVProVideoPlayer::IsLooping() {
    printf("AVProVideoPlayer::IsLooping stub\n");
    return false;
}

bool AVProVideoPlayer::HasVideo() {
    printf("AVProVideoPlayer::HasVideo stub\n");
    return false;
}

bool AVProVideoPlayer::HasAudio() {
    printf("AVProVideoPlayer::HasAudio stub\n");
    return false;
}

void AVProVideoPlayer::SetFocusProps(float a1, float a2) {
    printf("AVProVideoPlayer::SetFocusProps(%f, %f) stub\n", a1, a2);
}

void AVProVideoPlayer::SetFocusEnabled(bool enabled) {
    printf("AVProVideoPlayer::SetFocusEnabled(%d) stub\n", enabled);
}

void AVProVideoPlayer::SetFocusRotation(float a1, float a2, float a3, float a4) {
    printf("AVProVideoPlayer::SetFocusRotation(%f, %f, %f, %f) stub\n", a1, a2, a3, a4);
}

int32_t AVProVideoPlayer::GetPlayerIndex() {
    return m_playerIndex;
}

void AVProVideoPlayer::SetPlayerOptions(bool useFastOesPath, bool showPosterFrame) {
    printf("AVProVideoPlayer::SetPlayerOptions(%d, %d) stub\n", useFastOesPath, showPosterFrame);
}

void AVProVideoPlayer::CloseVideo() {
    printf("AVProVideo::CloseVideo stub\n");
}

void AVProVideoPlayer::Pause() {
    printf("AVProVideo::Pause stub\n");
}


bool AVProVideoPlayer::OpenVideoFromFile(const std::string &arg1, int64_t arg2, const std::shared_ptr<JNIObject> arg3, int32_t arg4) {
    printf("AVProVideoPlayer::OpenVideoFromFile(%s, %ld, %p, %d) stub!\n",
           arg1.c_str(), arg2, arg3.get(), arg4);

    return true;
}

void AVProVideoPlayer::SetDeinitialiseFlagged() {
    printf("AVProVideoPlayer::SetDeinitialiseFlagged stub!\n");
}

void AVProVideoPlayer::SetLooping(bool looping) {
    printf("AVProVideoPlayer::SetLooping(%d) stub!\n", looping);
}

void AVProVideoPlayer::initialize(int32_t playerType, bool useFastOesPath, bool showPosterFrame, int32_t arg4, bool arg5) {
    printf("AVProVideoPlayer::initialize(%d, %d, %d, %d, %d) stub!\n", playerType, useFastOesPath, showPosterFrame, arg4, arg5);
}

void AVProVideoPlayer::Deinitialise() {
    printf("AVProVideoPlayer::Deinitialise() stub!\n");
}

int32_t AVProVideoPlayer::GetWidth() {
    printf("AVProVideoPlayer::GetWidth() stub!\n");
    return 0;
}

int32_t AVProVideoPlayer::GetHeight() {
    printf("AVProVideoPlayer::GetHeight() stub!\n");
    return 0;
}

int32_t AVProVideoPlayer::GetTextureHandle() {
    printf("AVProVideoPlayer::GetTextureHandle() stub!\n");
    return 0;
}

int64_t AVProVideoPlayer::GetDurationMs() {
    printf("AVProVideoPlayer::GetDurationMs() stub!\n");
    return 0;
}

int32_t AVProVideoPlayer::GetLastErrorCode() {
    printf("AVProVideoPlayer::GetLastErrorCode() stub!\n");
    return 0;
}

int32_t AVProVideoPlayer::GetFrameCount() {
    printf("AVProVideoPlayer::GetFrameCount() stub!\n");
    return 0;
}

float AVProVideoPlayer::GetDisplayRate() {
    printf("AVProVideoPlayer::GetDisplayRate() stub!\n");
    return 0;
}

bool AVProVideoPlayer::CanPlay() {
    printf("AVProVideoPlayer::CanPlay() stub!\n");
    return false;
}

void AVProVideoPlayer::RendererSetup(int32_t glesVersion) {
    printf("AVProVideoPlayer::RendererSetup() stub!\n");
}

void AVProVideoPlayer::Render() {
    printf("AVProVideoPlayer::Render() stub!\n");
}

void AVProVideoPlayer::WaitForNewFrame() {
    printf("AVProVideoPlayer::WaitForNewFrame() stub!\n");
}

void AVProVideoPlayer::DeinitialiseRender() {
    printf("AVProVideoPlayer::DeinitialiseRender() stub!\n");
}

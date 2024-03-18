#include <VideoPlayer/Java/AVProVideoPlayer.h>
#include <Java/JNIClass.h>
#include <Java/JNIState.h>
#include <Java/JNIFloatArray.h>

AVProVideoPlayer::AVProVideoPlayer(int32_t playerIndex) : JNIObject(parent("com/RenderHeads/AVProVideo/AVProVideoPlayer")),
    m_playerIndex(playerIndex), m_renderingFullyInitialized(false) {

    printf("AVProVideoPlayer: created %p\n", this);

    m_mpv.setProperty("ytdl", false, true);
    m_mpv.setProperty("load-osd-console", false, true);
    m_mpv.setProperty("input-builtin-bindings", false);
    m_mpv.setProperty("input-default-bindings", false);
    m_mpv.setProperty("osc", false, true);
    m_mpv.setProperty("osd-level", INT64_C(0), true);
#ifndef _WIN32
    m_mpv.setProperty("hwdec", std::string("auto"));
#endif
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
    co->registerMethod("SetPlaybackRate", "(F)V", &AVProVideoPlayer::SetPlaybackRate);
    co->registerMethod("SetVolume", "(F)V", &AVProVideoPlayer::SetVolume);
    co->registerMethod("SetAudioPan", "(F)V", &AVProVideoPlayer::SetAudioPan);
    co->registerMethod("Play", "()V", &AVProVideoPlayer::Play);
    co->registerMethod("MuteAudio", "(Z)V", &AVProVideoPlayer::MuteAudio);
    co->registerMethod("GetTextureTransform", "()[F", &AVProVideoPlayer::GetTextureTransform);
    co->registerMethod("Seek", "(I)V", &AVProVideoPlayer::Seek);

    return co;
}

/*
 * This is called periodically to pump the video player's event queue.
 */
void AVProVideoPlayer::Update() {
    if(m_pendingFileToPlay.has_value()) {
        if(m_renderingFullyInitialized.load()) {
            auto file = std::move(*m_pendingFileToPlay);
            m_pendingFileToPlay.reset();

            printf("AVProVideoPlayer: rendering is initialized, starting to play: '%s'\n", file.c_str());

            if(file.empty())
                m_mpv.command("stop");
            else
                m_mpv.command("loadfile", file.c_str());
        }
    }

    m_mpv.dispatchEvents();
}

void AVProVideoPlayer::SetHeadRotation(float a1, float a2, float a3, float a4) {
    printf("AVProVideoPlayer::SetHeadRotation(%f, %f, %f, %f) stub\n", a1, a2, a3, a4);
}

int64_t AVProVideoPlayer::GetCurrentTimeMs() {
    return m_position;
}

float AVProVideoPlayer::GetSourceVideoFrameRate() {
    printf("AVProVideoPlayer::GetSourceVideoFrameRate stub\n");
    return 30.0f;
}

bool AVProVideoPlayer::IsPlaying() {
    return !m_coreIdle;
}

bool AVProVideoPlayer::IsPaused() {
    return m_pause;
}

bool AVProVideoPlayer::IsFinished() {
    return m_idleActive || m_eofReached;
}

bool AVProVideoPlayer::IsSeeking() {
    return m_seeking;
}

bool AVProVideoPlayer::IsBuffering() {
    return m_buffering;
}

bool AVProVideoPlayer::IsLooping() {
    return m_looping;
}

bool AVProVideoPlayer::HasVideo() {
    return m_hasVideo;
}

bool AVProVideoPlayer::HasAudio() {
    printf("AVProVideoPlayer::HasAudio stub\n");
    return true;
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
    m_pendingFileToPlay.emplace("");
}

void AVProVideoPlayer::Pause() {
    m_mpv.setProperty("pause", true);
}

bool AVProVideoPlayer::OpenVideoFromFile(const std::string &url, int64_t arg2, const std::shared_ptr<JNIObject> arg3, int32_t arg4) {
    std::string adjustedPath(url);

#if defined(_WIN32)
    /*
     * This path could be a result of incorrect resolution of a relative
     * path. If it is, then it'll contain multiple drive specifications.
     * Keep only the last one, and strip everything else.
     *
     * This also repairs the cases like '/C:/' that we produce in
     * androidPathFromWindowsPath below.
     */

    auto colonIndex = adjustedPath.find_last_of(':');
    if(colonIndex != std::string::npos && colonIndex != 0) {
        adjustedPath.erase(0, colonIndex - 1);
    }
#endif


    printf("AVProVideoPlayer::OpenVideoFromFile(%s, %ld, %p, %d) stub!\n",
           adjustedPath.c_str(), arg2, arg3.get(), arg4);

    Pause();

    m_pendingFileToPlay.emplace(std::move(adjustedPath));

    return true;
}

void AVProVideoPlayer::SetDeinitialiseFlagged() {
    printf("AVProVideoPlayer::SetDeinitialiseFlagged stub!\n");
}

void AVProVideoPlayer::SetLooping(bool looping) {
    m_mpv.setProperty("loop-file", looping);
}

void AVProVideoPlayer::initialize(int32_t playerType, bool useFastOesPath, bool showPosterFrame, int32_t arg4, bool arg5) {
    printf("AVProVideoPlayer::initialize(%d, %d, %d, %d, %d) stub!\n", playerType, useFastOesPath, showPosterFrame, arg4, arg5);

    m_mpv.initialize();

    m_mpv.observe<bool>("core-idle",   [this](const std::optional<bool> &idle) { m_coreIdle = idle.value_or(false); });
    m_mpv.observe<bool>("eof-reached", [this](const std::optional<bool> &eof) { m_eofReached = eof.value_or(false); });
    m_mpv.observe<bool>("pause",       [this](const std::optional<bool> &pause) { m_pause = pause.value_or(false); });
    m_mpv.observe<bool>("idle-active", [this](const std::optional<bool> &idleActive) { m_idleActive = idleActive.value_or(false); });
    m_mpv.observe<bool>("seeking",     [this](const std::optional<bool> &seeking) { m_seeking = seeking.value_or(false); });
    m_mpv.observe<bool>("paused-for-cache",
                        [this](const std::optional<bool> &buffering) { m_buffering = buffering.value_or(false); });
    m_mpv.observe<bool>("loop-file",   [this](const std::optional<bool> &looping) { m_looping = looping.value_or(false); });
    m_mpv.observe<double>("duration/full",
                          [this](const std::optional<double> &duration) { m_duration = duration.value_or(0) * 1e3; });
    m_mpv.observe<double>("time-pos/full",
                          [this](const std::optional<double> &position) { m_position = position.value_or(0) * 1000.0; });
    m_mpv.observe<int64_t>("width", [this](const std::optional<int64_t> &width) {

        m_hasVideo = width.has_value();
        m_nativeWidth = width.value_or(128);
    });
    m_mpv.observe<int64_t>("height", [this](const std::optional<int64_t> &height) {

        m_hasVideo = height.has_value();
        m_nativeHeight = height.value_or(64);
    });

    m_mpv.observe<double>("current-tracks/video/demux-fps", [this](const std::optional<double> &fps) {
        m_displayRate = fps.value_or(30.0f);
    });
}

void AVProVideoPlayer::Deinitialise() {
    printf("AVProVideoPlayer::Deinitialise() stub!\n");
}

int32_t AVProVideoPlayer::GetWidth() {
    return m_mpv.outputWidth();
}

int32_t AVProVideoPlayer::GetHeight() {
    return m_mpv.outputHeight();
}

int32_t AVProVideoPlayer::GetTextureHandle() {
    return m_mpv.outputTextureHandle();
}

int64_t AVProVideoPlayer::GetDurationMs() {
    return m_duration;
}

int32_t AVProVideoPlayer::GetLastErrorCode() {
    return 0;
}

/*
 * This is 'get number of frames pending'. For us, it's always 1, because mpv
 * paces to our actual rendering rate.
 */
int32_t AVProVideoPlayer::GetFrameCount() {
    return 1;
}

float AVProVideoPlayer::GetDisplayRate() {
    return m_displayRate;
}

bool AVProVideoPlayer::CanPlay() {
    return !m_idleActive;
}

void AVProVideoPlayer::RendererSetup(int32_t glesVersion) {
    (void)glesVersion;
    m_mpv.initializeRenderer();
    m_renderingFullyInitialized.store(true);
}

void AVProVideoPlayer::Render() {
    m_mpv.render(m_nativeWidth, m_nativeHeight);
}

void AVProVideoPlayer::WaitForNewFrame() {
    printf("AVProVideoPlayer::WaitForNewFrame() stub!\n");
}

void AVProVideoPlayer::DeinitialiseRender() {
    m_renderingFullyInitialized.store(false);
    m_mpv.destroyRenderer();
}

void AVProVideoPlayer::SetPlaybackRate(float rate) {
    m_mpv.setProperty("speed", static_cast<double>(rate));
}

void AVProVideoPlayer::SetVolume(float volume) {
    m_mpv.setProperty("volume", static_cast<double>(volume) * 100.0);
}

void AVProVideoPlayer::SetAudioPan(float pan) {
    printf("AVProVideoPlayer::SetAudioPan(%f) stub!\n", pan);
}

void AVProVideoPlayer::Play() {
    m_mpv.setProperty("pause", false);

    if(m_eofReached) {
        Seek(0);
    }
}

void AVProVideoPlayer::MuteAudio(bool mute) {
    m_mpv.setProperty("mute", mute);
}

std::shared_ptr<JNIObject> AVProVideoPlayer::GetTextureTransform() {
    /*
     * This will set up the unity transform, which is exactly what we want.
     */
    return nullptr;
}

void AVProVideoPlayer::Seek(int64_t positionMilliseconds) {
    m_mpv.command("seek",
                  std::to_string(std::max<int64_t>(0, positionMilliseconds) / 1000.0).c_str(),
                  "absolute");
}

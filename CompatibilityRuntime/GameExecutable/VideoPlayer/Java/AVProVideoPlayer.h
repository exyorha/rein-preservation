#ifndef VIDEO_PLAYER_JAVA_AV_PRO_VIDEO_PLAYER_H
#define VIDEO_PLAYER_JAVA_AV_PRO_VIDEO_PLAYER_H

#include <Java/JNIObject.h>

class AVProVideoPlayer final : public JNIObject {
public:
    explicit AVProVideoPlayer(int32_t playerIndex);
    ~AVProVideoPlayer();

    AVProVideoPlayer(const AVProVideoPlayer &other) = delete;
    AVProVideoPlayer &operator =(const AVProVideoPlayer &other) = delete;

    static std::shared_ptr<JNIClass> makeClass();

    void Deinitialise();

    void Update();
    void SetHeadRotation(float a1, float a2, float a3, float a4);
    int64_t GetCurrentTimeMs();
    float GetSourceVideoFrameRate();
    bool IsPlaying();
    bool IsPaused();
    bool IsFinished();
    bool IsSeeking();
    bool IsBuffering();
    bool IsLooping();
    void SetLooping(bool looping);
    bool HasVideo();
    bool HasAudio();
    void SetFocusProps(float a1, float a2);
    void SetFocusEnabled(bool enabled);
    void SetFocusRotation(float a1, float a2, float a3, float a4);
    int32_t GetPlayerIndex();
    void SetPlayerOptions(bool useFastOesPath, bool showPosterFrame);
    void CloseVideo();
    void Pause();
    bool OpenVideoFromFile(const std::string &arg1, int64_t arg2, const std::shared_ptr<JNIObject> arg3, int32_t arg4);
    void SetDeinitialiseFlagged();

    /*
     * Methods that are called via AVProVideoNativeBypass -> AVProMobileVideo
     * statics, they also exist in the original implementation.
     */
    int32_t GetWidth();
    int32_t GetHeight();
    int32_t GetTextureHandle();
    int64_t GetDurationMs();
    int32_t GetLastErrorCode();
    int32_t GetFrameCount();
    float GetDisplayRate();
    bool CanPlay();

    /*
     * Same, but called through AVProVideoNativeBypass's rendering callback.
     */
    void RendererSetup(int32_t glesVersion);
    void Render();
    void WaitForNewFrame();
    void DeinitialiseRender();


    /*
     * Ficitious methods that only exist in our implementation.
     */
    void initialize(int32_t playerType, bool useFastOesPath, bool showPosterFrame, int32_t arg4, bool arg5);

private:
    int32_t m_playerIndex;
};

#endif


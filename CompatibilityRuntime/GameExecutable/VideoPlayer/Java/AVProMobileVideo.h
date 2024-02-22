#ifndef VIDEO_PLAYER_JAVA_AV_PRO_MOBILE_VIDEO_H
#define VIDEO_PLAYER_JAVA_AV_PRO_MOBILE_VIDEO_H

#include <Java/JNIObject.h>

#include <memory>
#include <mutex>
#include <array>
#include <list>

class AVProVideoPlayer;

class AVProMobileVideo final : public JNIObject {
public:
    AVProMobileVideo();
    ~AVProMobileVideo();

    AVProMobileVideo(const AVProMobileVideo &other) = delete;
    AVProMobileVideo &operator =(const AVProMobileVideo &other) = delete;

    static std::shared_ptr<JNIClass> makeClass();

    std::shared_ptr<JNIObject> GetPluginVersion();

    void SetContext(std::shared_ptr<JNIObject> androidContextObject);

    std::shared_ptr<JNIObject> CreatePlayer(int32_t arg1, bool arg2, bool arg3, int32_t arg4, bool arg5);
    void DestroyPlayer(int32_t playerIndex);

    static void Deinitialise();

    /*
     * Called directly through AVLocal (see AVProVideoNativeBypass)
     */
    static int32_t GetWidth(int32_t playerIndex);
    static int32_t GetHeight(int32_t playerIndex);
    static int32_t GetTextureHandle(int32_t playerIndex);
    static int64_t GetDuration(int32_t playerIndex);
    static int32_t GetLastErrorCode(int32_t playerIndex);
    static int32_t GetFrameCount(int32_t playerIndex);
    static float GetVideoDisplayRate(int32_t playerIndex);
    static bool CanPlay(int32_t playerIndex);

    /*
     * Called from AVLocal's rendering callback, i.e., called from Unity.
     */
    static void RendererSetupPlayer(int32_t playerIndex, int32_t unityRendererType);
    static void RenderPlayer(int32_t playerIndex);
    static void RendererDestroyPlayers();
    static void WaitForNewFramePlayer(int32_t playerIndex);

private:
    struct PlayerRegistry {
    public:
        PlayerRegistry();
        ~PlayerRegistry();

        PlayerRegistry(const PlayerRegistry &other) = delete;
        PlayerRegistry &operator =(const PlayerRegistry &other) = delete;

        std::shared_ptr<AVProVideoPlayer> allocateNewVideoPlayer();
        bool destroyVideoPlayer(int32_t playerIndex);

        std::shared_ptr<AVProVideoPlayer> getVideoPlayer(int32_t playerIndex);

    private:
        std::mutex m_mutex;
        std::array<std::shared_ptr<AVProVideoPlayer>, 256> m_players;
    };

    struct RendererCleanupQueue {
    public:
        RendererCleanupQueue();
        ~RendererCleanupQueue();

        RendererCleanupQueue(const RendererCleanupQueue &other) = delete;
        RendererCleanupQueue &operator =(const RendererCleanupQueue &other) = delete;

        void enqueue(const std::shared_ptr<AVProVideoPlayer> &player);
        void purge();

    private:
        std::mutex m_mutex;
        std::list<std::shared_ptr<AVProVideoPlayer>> m_queue;
    };

    static std::shared_ptr<JNIObject> create();

    static PlayerRegistry m_registry;
    static RendererCleanupQueue m_rendererCleanupQueue;
};

#endif


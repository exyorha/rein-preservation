#include <VideoPlayer/Java/AVProMobileVideo.h>
#include <Java/JNIClass.h>
#include <Java/JNIString.h>
#include <Java/JNIState.h>
#include <VideoPlayer/Java/AVProVideoPlayer.h>

AVProMobileVideo::PlayerRegistry AVProMobileVideo::m_registry;
AVProMobileVideo::RendererCleanupQueue AVProMobileVideo::m_rendererCleanupQueue;

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

    co->registerMethod("RendererSetupPlayer", "(II)V", &AVProMobileVideo::RendererSetupPlayer);
    co->registerMethod("RendererPlayer", "(I)V", &AVProMobileVideo::RenderPlayer);
    co->registerMethod("RendererDestroyPlayers", "()V", &AVProMobileVideo::RendererDestroyPlayers);
    co->registerMethod("WaitForNewFramePlayer", "(I)V", &AVProMobileVideo::WaitForNewFramePlayer);

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

    auto player = m_registry.allocateNewVideoPlayer();
    if(!player)
        return nullptr;

    struct PartiallyInitializedPlayer {
    public:
        bool fullyInitialized = false;
        std::shared_ptr<AVProVideoPlayer> *playerPointer = nullptr;

        ~PartiallyInitializedPlayer() {
            if(!fullyInitialized && playerPointer) {
                m_registry.destroyVideoPlayer((*playerPointer)->GetPlayerIndex());
                playerPointer->reset();
            }
        }
    } initializationGuard;

    initializationGuard.playerPointer = &player;

    player->initialize(playerType, useFastOesPath, showPosterFrame, arg4, arg5);

    initializationGuard.fullyInitialized = true;
    return player;
}

void AVProMobileVideo::DestroyPlayer(int32_t playerIndex) {
    auto player = m_registry.getVideoPlayer(playerIndex);
    if(player) {
        player->Deinitialise();

        m_rendererCleanupQueue.enqueue(player);

        m_registry.destroyVideoPlayer(playerIndex);
    }
}

int32_t AVProMobileVideo::GetWidth(int32_t playerIndex) {
    auto player = m_registry.getVideoPlayer(playerIndex);
    if(player) {
        return player->GetWidth();
    } else {
        return 0;
    }
}

int32_t AVProMobileVideo::GetHeight(int32_t playerIndex) {
    auto player = m_registry.getVideoPlayer(playerIndex);
    if(player) {
        return player->GetHeight();
    } else {
        return 0;
    }
}

int32_t AVProMobileVideo::GetTextureHandle(int32_t playerIndex) {
    auto player = m_registry.getVideoPlayer(playerIndex);
    if(player) {
        return player->GetTextureHandle();
    } else {
        return 0;
    }
}

int64_t AVProMobileVideo::GetDuration(int32_t playerIndex) {
    auto player = m_registry.getVideoPlayer(playerIndex);
    if(player) {
        return player->GetDurationMs();
    } else {
        return 0;
    }
}

int32_t AVProMobileVideo::GetLastErrorCode(int32_t playerIndex) {
    auto player = m_registry.getVideoPlayer(playerIndex);
    if(player) {
        return player->GetLastErrorCode();
    } else {
        return 0;
    }
}

int32_t AVProMobileVideo::GetFrameCount(int32_t playerIndex) {
    auto player = m_registry.getVideoPlayer(playerIndex);
    if(player) {
        return player->GetFrameCount();
    } else {
        return 0;
    }
}

float AVProMobileVideo::GetVideoDisplayRate(int32_t playerIndex) {
    auto player = m_registry.getVideoPlayer(playerIndex);
    if(player) {
        return player->GetDisplayRate();
    } else {
        return 0;
    }
}

bool AVProMobileVideo::CanPlay(int32_t playerIndex) {
    auto player = m_registry.getVideoPlayer(playerIndex);
    if(player) {
        return player->CanPlay();
    } else {
        return 0;
    }
}

void AVProMobileVideo::Deinitialise() {

}

void AVProMobileVideo::RendererSetupPlayer(int32_t playerIndex, int32_t unityRendererType) {
    (void)unityRendererType;

    auto player = m_registry.getVideoPlayer(playerIndex);
    if(player) {
        player->RendererSetup(3);
    }
}

void AVProMobileVideo::RenderPlayer(int32_t playerIndex) {
    auto player = m_registry.getVideoPlayer(playerIndex);
    if(player) {
        player->Render();
    }
}

void AVProMobileVideo::RendererDestroyPlayers() {
    m_rendererCleanupQueue.purge();
}

void AVProMobileVideo::WaitForNewFramePlayer(int32_t playerIndex) {
    auto player = m_registry.getVideoPlayer(playerIndex);

    if(player) {
        player->WaitForNewFrame();
    }
}

AVProMobileVideo::PlayerRegistry::PlayerRegistry() = default;

AVProMobileVideo::PlayerRegistry::~PlayerRegistry() = default;

std::shared_ptr<AVProVideoPlayer> AVProMobileVideo::PlayerRegistry::allocateNewVideoPlayer() {
    std::unique_lock<std::mutex> locker(m_mutex);

    for(size_t index = 0, size = m_players.size(); index < size; index++) {
        if(!m_players[index]) {
            auto player = std::make_shared<AVProVideoPlayer>(index);
            m_players[index] = player;
            return player;
        }
    }

    return nullptr;
}

bool AVProMobileVideo::PlayerRegistry::destroyVideoPlayer(int32_t playerIndex) {
    if(playerIndex < 0 || playerIndex >= m_players.size())
        return false;

    std::unique_lock<std::mutex> locker(m_mutex);
    if(!m_players[playerIndex])
        return false;

    m_players[playerIndex].reset();

    return true;
}

std::shared_ptr<AVProVideoPlayer> AVProMobileVideo::PlayerRegistry::getVideoPlayer(int32_t playerIndex) {
    if(playerIndex < 0 || playerIndex >= m_players.size())
        return nullptr;

    std::unique_lock<std::mutex> locker(m_mutex);
    return m_players[playerIndex];
}

AVProMobileVideo::RendererCleanupQueue::RendererCleanupQueue() = default;

AVProMobileVideo::RendererCleanupQueue::~RendererCleanupQueue() = default;

void AVProMobileVideo::RendererCleanupQueue::enqueue(const std::shared_ptr<AVProVideoPlayer> &player) {
    std::unique_lock<std::mutex> locker(m_mutex);

    m_queue.emplace_back(player);
}

void AVProMobileVideo::RendererCleanupQueue::purge() {
    std::unique_lock<std::mutex> locker(m_mutex);

    while(!m_queue.empty()) {
        auto player = m_queue.front();
        m_queue.pop_front();

        locker.unlock();

        player->DeinitialiseRender();

        locker.lock();
    }
}

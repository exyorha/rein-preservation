#ifndef VIDEO_PLAYER_MPV_PLAYER_H
#define VIDEO_PLAYER_MPV_PLAYER_H

#include <VideoPlayer/VideoRenderingOpenGLAPISet.h>
#include <VideoPlayer/MPVPropertyFormat.h>
#include <VideoPlayer/MPVPropertyObserver.h>

#include <array>

class MPVPlayer {
public:
    MPVPlayer();
    ~MPVPlayer();

    MPVPlayer(const MPVPlayer &other) = delete;
    MPVPlayer &operator =(const MPVPlayer &other) = delete;

    void dispatchEvents();

    void initialize();

    template<typename T>
    std::optional<T> getProperty(const char *name) {
        typename MPVPropertyFormat<T>::mpvType storage;

        if(!getPropertyp(name, MPVPropertyFormat<T>::format, &storage))
            return std::nullopt;

        return MPVPropertyFormat<T>::unpack(storage);
    }

    template<typename T>
    void setProperty(const char *name, const T &value) {
        typename MPVPropertyFormat<T>::mpvType storage = MPVPropertyFormat<T>::pack(value);

        setPropertyp(name, MPVPropertyFormat<T>::format, &storage);
    }

    template<typename... Args>
    inline void command(Args... args) {
        std::array<const char *, sizeof...(Args) + 1> arguments{ args..., nullptr };

        commandv(arguments.data());
    }

    void commandv(const char **arguments);

    bool getPropertyp(const char *name, mpv_format format, void *data);
    void setPropertyp(const char *name, mpv_format format, const void *data);

    /*
     * In contrast to all the other methods, the following ones are called on the Unity
     * render thread.
     */
    void initializeRenderer();
    void destroyRenderer();
    void render(int32_t desiredWidth, int32_t desiredHeight);

    inline uint32_t outputWidth() const {
        return m_currentlyAllocatedTextureWidth;
    }

    inline uint32_t outputHeight() {
        return m_currentlyAllocatedTextureHeight;
    }

    inline uint32_t outputTextureHandle() const {
        return m_texture;
    }

    template<typename T>
    void observe(const char *propertyName, std::function<void(const std::optional<T> &value)> &&observer) {
        auto observerObject = static_cast<MPVPropertyObserver<T> *>(
            m_propertyObservers.emplace_back(std::move(std::make_unique<MPVPropertyObserver<T>>(std::move(observer)))).get()
        );

        observep(observerObject, propertyName, MPVPropertyFormat<T>::format);

        observerObject->invoke(getProperty<T>(propertyName));
    }

    void observep(MPVBasePropertyObserver *observer, const char *propertyName, mpv_format format);

private:
    void handleMPVEvent(mpv_event *event);

    static void *mpvOpenGLGetProcAddress(void *ctx, const char *name);

    MPVHandlePtr m_mpv;
    mpv_render_context *m_renderer;
    std::vector<std::unique_ptr<MPVBasePropertyObserver>> m_propertyObservers;

    /*
     * Fields that are accessed on the rendering thread *only*.
     */
    static std::optional<VideoRenderingOpenGLAPISet> m_openglAPI;
    GLuint m_framebuffer;
    GLuint m_texture;
    unsigned int m_currentlyAllocatedTextureWidth;
    unsigned int m_currentlyAllocatedTextureHeight;
};

#endif

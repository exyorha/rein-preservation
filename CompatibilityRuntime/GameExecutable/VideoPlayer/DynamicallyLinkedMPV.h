#ifndef VIDEO_PLAYER_DYNAMICALLY_LINKED_MPV_H
#define VIDEO_PLAYER_DYNAMICALLY_LINKED_MPV_H

#include <optional>

#include <mpv/client.h>
#include <mpv/render.h>
#include <mpv/render_gl.h>

#include <VideoPlayer/PlatformDynamicLibrary.h>

class DynamicallyLinkedMPV {
public:
    DynamicallyLinkedMPV();
    ~DynamicallyLinkedMPV();

    DynamicallyLinkedMPV(const DynamicallyLinkedMPV &other) = delete;
    DynamicallyLinkedMPV &operator =(const DynamicallyLinkedMPV &other) = delete;

    static inline bool isInitialized() noexcept {
        return m_instance.has_value();
    }

    static inline void initialize() {
        m_instance.emplace();
    }

    static inline void deinitialize() {
        m_instance.reset();
    }

    static const DynamicallyLinkedMPV &get();

    unsigned long (*mpv_client_api_version)(void);

    mpv_handle *(*mpv_create)(void);
    int (*mpv_request_log_messages)(mpv_handle *ctx, const char *min_level);
    int (*mpv_initialize)(mpv_handle *ctx);
    mpv_event *(*mpv_wait_event)(mpv_handle *ctx, double timeout);
    int (*mpv_command)(mpv_handle *ctx, const char **args);
    int (*mpv_get_property)(mpv_handle *ctx, const char *name, mpv_format format, void *data);
    int (*mpv_set_property)(mpv_handle *ctx, const char *name, mpv_format format, void *data);
    int (*mpv_observe_property)(mpv_handle *mpv, uint64_t reply_userdata, const char *name, mpv_format format);
    int (*mpv_render_context_create)(mpv_render_context **res, mpv_handle *mpv, mpv_render_param *params);
    void (*mpv_render_context_free)(mpv_render_context *ctx);
    int (*mpv_render_context_render)(mpv_render_context *ctx, mpv_render_param *params);
    void (*mpv_terminate_destroy)(mpv_handle *ctx);
    const char *(*mpv_error_string)(int error);

private:
    template<typename Result, typename... Args>
    inline void bindFunction(const char *name, Result (*&pointer)(Args... args)) {
        pointer = reinterpret_cast<Result (*)(Args...)>(bindFunction(name));
    }

    void *bindFunction(const char *name);

    static std::optional<DynamicallyLinkedMPV> m_instance;
    std::optional<PlatformDynamicLibrary> m_library;
};

#endif

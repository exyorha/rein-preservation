#include "VideoPlayer/MPVBasePropertyObserver.h"
#include <GLES2/gl2.h>
#include <VideoPlayer/MPVPlayer.h>
#include <VideoPlayer/MPVError.h>

#include <mpv/client.h>
#include <mpv/render.h>
#include <vector>
#include <cstring>

#ifndef _WIN32
#include <GLES/SDL/SDLWrapper.h>
#include <GLES/SDL/SDLGLESImplementation.h>
#include <GLES/SDL/RealSDLSymbols.h>
#endif

std::optional<VideoRenderingOpenGLAPISet> MPVPlayer::m_openglAPI;

MPVPlayer::MPVPlayer() : m_renderer(nullptr), m_framebuffer(0),
    m_texture(0), m_currentlyAllocatedTextureWidth(0), m_currentlyAllocatedTextureHeight(0) {

    {
        auto raw = mpv_create();
        if(!raw)
            throw std::bad_alloc();

        m_mpv.reset(raw);
    }

    auto result = mpv_request_log_messages(m_mpv.get(), "v");
    if(result < 0)
        throw MPVError(result);

    setProperty("vo", std::string("libmpv"));
    setProperty("keep-open", true);

}

MPVPlayer::~MPVPlayer() = default;

void MPVPlayer::initialize() {
    auto result = mpv_initialize(m_mpv.get());
    if(result < 0)
        throw MPVError(result);

    dispatchEvents();
}

void MPVPlayer::dispatchEvents() {
    mpv_event *event;
    while(true) {
        event = mpv_wait_event(m_mpv.get(), 0.0);

        if(event->event_id == MPV_EVENT_NONE)
            break;

        handleMPVEvent(event);
    }
}

void MPVPlayer::handleMPVEvent(mpv_event *event) {
    if(event->event_id == MPV_EVENT_LOG_MESSAGE) {
        auto message = static_cast<mpv_event_log_message *>(event->data);
        printf("mpv/%s: %s: %s", message->prefix, message->level, message->text);
    } else if(event->event_id == MPV_EVENT_PROPERTY_CHANGE) {
        auto property = static_cast<mpv_event_property *>(event->data);

        reinterpret_cast<MPVBasePropertyObserver *>(event->reply_userdata)->processEvent(property);
    } else {
        printf("MPV event %d\n", event->event_id);
    }
}

void MPVPlayer::commandv(const char **arguments) {

    printf("MPVPlayer::commandv: ");
    for(auto word = arguments; *word; word++) {
        printf("%s ", *word);
    }

    printf("\n");

    auto result = mpv_command(m_mpv.get(), arguments);

    dispatchEvents();

    if(result < 0)
        throw MPVError(result);

}

bool MPVPlayer::getPropertyp(const char *name, mpv_format format, void *data) {
    printf("MPVPlayer: querying a property: '%s'\n", name);

    auto result = mpv_get_property(m_mpv.get(), name, format, data);

    dispatchEvents();

    if(result == MPV_ERROR_PROPERTY_UNAVAILABLE)
        return false;

    if(result < 0)
        throw MPVError(result);

    return true;
}

void MPVPlayer::setPropertyp(const char *name, mpv_format format, const void *data) {
    printf("MPVPlayer: setting a property: '%s'\n", name);

    auto result = mpv_set_property(m_mpv.get(), name, format, const_cast<void *>(data));

    dispatchEvents();

    if(result < 0)
        throw MPVError(result);
}

void MPVPlayer::observep(MPVBasePropertyObserver *observer, const char *propertyName, mpv_format format) {
    auto result = mpv_observe_property(m_mpv.get(), reinterpret_cast<uintptr_t>(observer), propertyName, format);

    dispatchEvents();

    if(result < 0)
        throw MPVError(result);
}

void MPVPlayer::initializeRenderer() {
    printf("MPVPlayer::initializeRenderer!\n");

    /*
     * This method is called on the render thread.
     */

    std::vector<mpv_render_param> params;

    //int advancedControl = 1;
    //params.emplace_back(mpv_render_param{ MPV_RENDER_PARAM_ADVANCED_CONTROL, &advancedControl });

    params.emplace_back(mpv_render_param{ MPV_RENDER_PARAM_API_TYPE, const_cast<char *>(MPV_RENDER_API_TYPE_OPENGL) });

    mpv_opengl_init_params glParams = {
        .get_proc_address = mpvOpenGLGetProcAddress,
        .get_proc_address_ctx = nullptr
    };

    if(!m_openglAPI.has_value()) {
        m_openglAPI.emplace(&glParams);
    }

    params.emplace_back(mpv_render_param{ MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &glParams });

#ifndef _WIN32
    auto window = getSDLWindowForWebView();
    SDL_SysWMinfo wmInfo;
    memset(&wmInfo, 0, sizeof(wmInfo));
    wmInfo.version.major = 2;
    wmInfo.version.minor = 0;
    if(window && RealSDLSymbols::getSingleton().realGetWindowWMInfo(window, &wmInfo) && wmInfo.subsystem == SDL_SYSWM_X11) {
        params.emplace_back(mpv_render_param{ MPV_RENDER_PARAM_X11_DISPLAY, wmInfo.info.x11.display });
    }
#endif

    params.emplace_back(mpv_render_param{ MPV_RENDER_PARAM_INVALID, nullptr });

    auto result = mpv_render_context_create(&m_renderer, m_mpv.get(), params.data());
    if(result < 0)
        throw MPVError(result);

    const auto &glapi = *m_openglAPI;
    glapi.glGenFramebuffers(1, &m_framebuffer);


    if(glapi.glLabelObjectEXT) {
        glapi.glLabelObjectEXT(GL_FRAMEBUFFER, m_framebuffer, -1, "MPV final framebuffer");
    }

    printf("MPVPlayer::initializeRenderer finished!\n");
}

void MPVPlayer::destroyRenderer() {
    printf("MPVPlayer::destroyRenderer!\n");

    /*
     * This method is called on the render thread.
     */

    if(m_openglAPI.has_value()) {
        const auto &glapi = *m_openglAPI;

        if(m_framebuffer != 0) {
            glapi.glDeleteFramebuffers(1, &m_framebuffer);
            m_framebuffer = 0;
        }
    }

    if(m_renderer) {

        mpv_render_context_free(m_renderer);

        m_renderer = nullptr;
    }
}

void MPVPlayer::render(int32_t desiredWidth, int32_t desiredHeight) {
    /*
     * This method is called on the render thread.
     */

    const auto &glapi = *m_openglAPI;

    if(m_framebuffer == 0 || !m_renderer)
        return;

    if(m_currentlyAllocatedTextureWidth != desiredWidth ||
        m_currentlyAllocatedTextureHeight != desiredHeight ||
        m_texture == 0) {

        if(m_texture != 0) {
            glapi.glDeleteTextures(1, &m_texture);
            m_texture = 0;
        }

        if(desiredWidth <= 0 || desiredHeight == 0)
            return;

        glapi.glGenTextures(1, &m_texture);

        if(glapi.glLabelObjectEXT) {
            glapi.glLabelObjectEXT(GL_TEXTURE, m_texture, -1, "MPV final texture");
        }

        glapi.glActiveTexture(GL_TEXTURE0);
        glapi.glBindTexture(GL_TEXTURE_2D, m_texture);
        glapi.glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, desiredWidth, desiredHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glapi.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        m_currentlyAllocatedTextureWidth = desiredWidth;
        m_currentlyAllocatedTextureHeight = desiredHeight;
        glapi.glBindTexture(GL_TEXTURE_2D, 0);

        glapi.glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
        glapi.glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);
        glapi.glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }

    mpv_opengl_fbo fbo = {
        .fbo = static_cast<int>(m_framebuffer),
        .w = static_cast<int>(m_currentlyAllocatedTextureWidth),
        .h = static_cast<int>(m_currentlyAllocatedTextureHeight),
        .internal_format = GL_RGBA
    };

    int flipY = 1;

    std::array<mpv_render_param, 3> params{
        mpv_render_param{ MPV_RENDER_PARAM_OPENGL_FBO, &fbo },
        mpv_render_param{ MPV_RENDER_PARAM_FLIP_Y, &flipY },
        mpv_render_param{ MPV_RENDER_PARAM_INVALID, nullptr }
    };

    mpv_render_context_render(m_renderer, params.data());
}

#ifndef _WIN32

void *MPVPlayer::mpvOpenGLGetProcAddress(void *ctx, const char *name) {
    (void)ctx;

    if(!SelectedSDLGLESImplementation)
        return nullptr;

    return SelectedSDLGLESImplementation->GetProcAddress(name);
}

#endif


#ifndef VIDEO_PLAYER_MPV_TYPES_H
#define VIDEO_PLAYER_MPV_TYPES_H

#include <mpv/client.h>
#include <mpv/stream_cb.h>
#include <mpv/render.h>
#include <mpv/render_gl.h>

#include <memory>

struct MPVHandleDeleter {
    inline void operator()(mpv_handle *handle) const {
        mpv_terminate_destroy(handle);
    }
};

using MPVHandlePtr = std::unique_ptr<mpv_handle, MPVHandleDeleter>;

#endif

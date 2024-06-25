#ifndef VIDEO_PLAYER_MPV_TYPES_H
#define VIDEO_PLAYER_MPV_TYPES_H

#include <VideoPlayer/DynamicallyLinkedMPV.h>

#include <memory>

struct MPVHandleDeleter {
    inline void operator()(mpv_handle *handle) const {
        DynamicallyLinkedMPV::get().mpv_terminate_destroy(handle);
    }
};

using MPVHandlePtr = std::unique_ptr<mpv_handle, MPVHandleDeleter>;

#endif

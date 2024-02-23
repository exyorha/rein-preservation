#ifndef VIDEO_PLAYER_MPV_PROPERTY_FORMAT_H
#define VIDEO_PLAYER_MPV_PROPERTY_FORMAT_H

#include <VideoPlayer/MPVTypes.h>
#include <mpv/client.h>

template<typename T>
struct MPVPropertyFormat {};

template<>
struct MPVPropertyFormat<bool> {
    static constexpr mpv_format format = MPV_FORMAT_FLAG;
    using mpvType = int;

    static inline int pack(bool src) {
        return src;
    }

    static inline bool unpack(int src) {
        return src != 0;
    }
};

template<>
struct MPVPropertyFormat<double> {
    static constexpr mpv_format format = MPV_FORMAT_DOUBLE;
    using mpvType = double;

    static inline double pack(double src) {
        return src;
    }

    static inline double unpack(double src) {
        return src;
    }
};

template<>
struct MPVPropertyFormat<int64_t> {
    static constexpr mpv_format format = MPV_FORMAT_INT64;
    using mpvType = int64_t;

    static inline int64_t pack(int64_t src) {
        return src;
    }

    static inline int64_t unpack(int64_t src) {
        return src;
    }
};

template<>
struct MPVPropertyFormat<std::string> {
    static constexpr mpv_format format = MPV_FORMAT_STRING;
    using mpvType = std::string;

    static inline const char *pack(const std::string & src) {
        return src.c_str();
    }
};

#endif

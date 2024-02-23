#ifndef VIDEO_PLAYER_MPV_PROPERTY_OBSERVER_H
#define VIDEO_PLAYER_MPV_PROPERTY_OBSERVER_H

#include <VideoPlayer/MPVBasePropertyObserver.h>
#include <VideoPlayer/MPVPropertyFormat.h>

#include <functional>
#include <optional>
#include <cstdio>

template<typename T>
class MPVPropertyObserver final : public MPVBasePropertyObserver {
public:
    explicit MPVPropertyObserver(std::function<void(const std::optional<T> &value)> &&handler) :
        m_handler(std::move(handler)) {

    }

    ~MPVPropertyObserver() override {

    }

    void invoke(const std::optional<T> &value) {
        m_handler(value);
    }

    void processEvent(const mpv_event_property *event) override {
        if(event->format == MPV_FORMAT_NONE) {
            m_handler(std::nullopt);
        } else {
            if(event->format != MPVPropertyFormat<T>::format)
                throw std::runtime_error("unexpected format of an observed property");

            auto value = MPVPropertyFormat<T>::unpack(*static_cast<const typename MPVPropertyFormat<T>::mpvType *>(event->data));
            m_handler(value);
        }
    }

private:
    std::function<void(const std::optional<T> &value)> m_handler;
};

#endif


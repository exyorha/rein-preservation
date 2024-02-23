#ifndef VIDEO_PLAYER_MPV_BASE_PROPERTY_OBSERVER_H
#define VIDEO_PLAYER_MPV_BASE_PROPERTY_OBSERVER_H

struct mpv_event_property;

class MPVBasePropertyObserver {
protected:
    MPVBasePropertyObserver();

public:
    virtual ~MPVBasePropertyObserver();

    MPVBasePropertyObserver(const MPVBasePropertyObserver &other) = delete;
    MPVBasePropertyObserver &operator =(const MPVBasePropertyObserver &other) = delete;

    virtual void processEvent(const mpv_event_property *event) = 0;
};

#endif

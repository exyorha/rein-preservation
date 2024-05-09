#ifndef INPUT_TOUCH_INPUT_RECEIVER_H
#define INPUT_TOUCH_INPUT_RECEIVER_H

struct UnityEngine_Touch;

class TouchInputReceiver {
protected:
    TouchInputReceiver();
    ~TouchInputReceiver();

public:
    TouchInputReceiver(const TouchInputReceiver &other) = delete;
    TouchInputReceiver &operator =(const TouchInputReceiver &other) = delete;

    virtual bool touchUpdated(const UnityEngine_Touch &touch) = 0;
};

#endif


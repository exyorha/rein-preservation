#ifndef INPUT_UNITY_TOUCH_INPUT_RECEIVER_H
#define INPUT_UNITY_TOUCH_INPUT_RECEIVER_H

#include <Input/TouchInputReceiver.h>

class UnityTouchInputReceiver final : public TouchInputReceiver {
public:
    UnityTouchInputReceiver();
    ~UnityTouchInputReceiver();

    bool touchUpdated(const UnityEngine_Touch &touch) override;
};

#endif


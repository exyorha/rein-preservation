#include "UnityTouchInputReceiver.h"

UnityTouchInputReceiver::UnityTouchInputReceiver() = default;

UnityTouchInputReceiver::~UnityTouchInputReceiver() = default;

bool UnityTouchInputReceiver::touchUpdated(const UnityEngine_Touch &touch) {
    return true;
}

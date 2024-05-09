#include "Input/TouchEmulator.h"
#include "Input/TouchInputReceiver.h"
#include "Input/TouchInputAreaReceiver.h"

#include <algorithm>
#include <cmath>

TouchEmulator::TouchEmulator() : m_backgroundLayerInputReceiver(nullptr) {

}

TouchEmulator::~TouchEmulator() = default;

void TouchEmulator::setBackgroundLayerInputReceiver(TouchInputReceiver *backgroundLayerInputReceiver) {
    if(m_backgroundLayerInputReceiver != backgroundLayerInputReceiver) {
        if(m_backgroundLayerInputReceiver) {
            clearTouchInputReceiver(m_backgroundLayerInputReceiver);
        }

        m_backgroundLayerInputReceiver = backgroundLayerInputReceiver;
    }
}

void TouchEmulator::registerAreaReceiver(TouchInputAreaReceiver *receiver) {
    m_areaReceivers.emplace_back(receiver);
}

void TouchEmulator::unregisterAreaReceiver(TouchInputAreaReceiver *receiver) {
    clearTouchInputReceiver(receiver);

    m_areaReceivers.erase(std::remove(m_areaReceivers.begin(), m_areaReceivers.end(), receiver), m_areaReceivers.end());
}

TouchInputReceiver *TouchEmulator::determineReceiverForPoint(int32_t x, int32_t y) const {
    for(auto receiver: m_areaReceivers) {
        if(receiver->coversPoint(x, y))
            return receiver;
    }

    return m_backgroundLayerInputReceiver;
}

bool TouchEmulator::emulateTouchInput(float x, float y, bool pressed) {

    if(pressed) {
        if(!m_currentTouch.has_value() || m_currentTouch->m_Phase == UnityEngine_TouchPhase::Ended) {
            auto &touch = m_currentTouch.emplace(RoutedTouch{});

            touch.m_FingerId = 0;
            touch.m_Position.x = x;
            touch.m_Position.y = y;
            touch.m_RawPosition = touch.m_Position;
            touch.m_PositionDelta.x = 0.0f;
            touch.m_PositionDelta.y = 0.0f;
            touch.m_TimeDelta = 0.0f;
            touch.m_TapCount = 0;
            touch.m_Phase = UnityEngine_TouchPhase::Began;
            touch.m_Type = UnityEngine_TouchType::Direct;
            touch.m_Pressure = 1.0f;
            touch.m_maximumPossiblePressure = 1.0f;
            touch.m_Radius = 1.0f;
            touch.m_RadiusVariance = 0.0f;
            touch.m_AltitudeAngle = static_cast<float>(M_PI / 2.0f);
            touch.m_AzimuthAngle = 0.0f;
            touch.receiver = determineReceiverForPoint(roundf(x), roundf(y));

        } else {
            auto &touch = *m_currentTouch;

            touch.m_PositionDelta.x = x - touch.m_Position.x;
            touch.m_PositionDelta.y = y - touch.m_Position.y;

            if(hypotf(touch.m_PositionDelta.x, touch.m_PositionDelta.y) <= 3.0f) {
                touch.m_Phase = UnityEngine_TouchPhase::Stationary;
            } else {
                touch.m_Phase = UnityEngine_TouchPhase::Moved;
            }

            touch.m_Position.x = x;
            touch.m_Position.y = y;
        }

    } else {
        if(m_currentTouch.has_value()) {
            if(m_currentTouch->m_Phase == UnityEngine_TouchPhase::Ended)
                m_currentTouch.reset();
            else {
                m_currentTouch->m_Phase = UnityEngine_TouchPhase::Ended;
            }
        }
    }

    bool passMouse = false;

    if(m_currentTouch.has_value() && m_currentTouch->receiver) {
        passMouse = m_currentTouch->receiver->touchUpdated(*m_currentTouch);
    }

    return passMouse;
}

const TouchPacket *TouchEmulator::currentTouch(TouchInputReceiver *forReceiver) const {
    if(!m_currentTouch.has_value() || (forReceiver != nullptr && forReceiver != m_currentTouch->receiver))
        return nullptr;

    return &*m_currentTouch;
}

void TouchEmulator::clearTouchInputReceiver(TouchInputReceiver *receiver) {
    if(m_currentTouch.has_value() && m_currentTouch->receiver == receiver) {
        m_currentTouch->receiver = nullptr;
    }
}

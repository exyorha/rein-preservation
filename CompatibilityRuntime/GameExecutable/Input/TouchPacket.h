#ifndef INPUT_TOUCH_PACKET_H
#define INPUT_TOUCH_PACKET_H

#include <cstdint>

struct UnityEngine_Vector2 {
    float x;
    float y;
};

struct UnityEngine_Vector3 {
    float x;
    float y;
    float z;
};

enum class UnityEngine_TouchPhase : int32_t {
    Began = 0,
    Moved = 1,
    Stationary = 2,
    Ended = 3,
    Canceled = 4
};

enum class UnityEngine_TouchType : int32_t {
    Direct = 0,
    Indirect = 1,
    Stylus = 2
};


struct UnityEngine_Touch {
    int32_t m_FingerId;
    UnityEngine_Vector2 m_Position;
    UnityEngine_Vector2 m_RawPosition;
    UnityEngine_Vector2 m_PositionDelta;
    float m_TimeDelta;
    int32_t m_TapCount;
    UnityEngine_TouchPhase m_Phase;
    UnityEngine_TouchType m_Type;
    float m_Pressure;
    float m_maximumPossiblePressure;
    float m_Radius;
    float m_RadiusVariance;
    float m_AltitudeAngle;
    float m_AzimuthAngle;
};
static_assert(sizeof(UnityEngine_Touch) == 0x44);

using TouchPacket = UnityEngine_Touch;

#endif

#ifndef INPUT_TOUCH_EMULATOR_H
#define INPUT_TOUCH_EMULATOR_H

#include <vector>
#include <cstdint>

#include <Input/TouchPacket.h>
#include <optional>

class TouchInputReceiver;
class TouchInputAreaReceiver;

class TouchEmulator {
public:
    TouchEmulator();
    ~TouchEmulator();

    TouchEmulator(const TouchEmulator &other) = delete;
    TouchEmulator &operator =(const TouchEmulator &other) = delete;

    inline TouchInputReceiver *backgroundLayerInputReceiver() const {
        return m_backgroundLayerInputReceiver;
    }

    void setBackgroundLayerInputReceiver(TouchInputReceiver *backgroundLayerInputReceiver);

    void registerAreaReceiver(TouchInputAreaReceiver *receiver);
    void unregisterAreaReceiver(TouchInputAreaReceiver *receiver);

    bool emulateTouchInput(float x, float y, bool pressed);

    const TouchPacket *currentTouch(TouchInputReceiver *forReceiver = nullptr) const;

private:
    void clearTouchInputReceiver(TouchInputReceiver *receiver);

    struct RoutedTouch final : public TouchPacket {
        TouchInputReceiver *receiver = nullptr;
    };

    TouchInputReceiver *determineReceiverForPoint(int32_t x, int32_t y) const;

    TouchInputReceiver *m_backgroundLayerInputReceiver;
    std::vector<TouchInputAreaReceiver *> m_areaReceivers;
    std::optional<RoutedTouch> m_currentTouch;
};

#endif

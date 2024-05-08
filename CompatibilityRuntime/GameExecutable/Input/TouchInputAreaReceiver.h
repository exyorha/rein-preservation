#ifndef INPUT_TOUCH_INPUT_AREA_RECEIVER_H
#define INPUT_TOUCH_INPUT_AREA_RECEIVER_H

#include <cstdint>

#include <Input/TouchInputReceiver.h>

class TouchInputAreaReceiver : public TouchInputReceiver {
protected:
    TouchInputAreaReceiver();
    ~TouchInputAreaReceiver();

public:

    bool coversPoint(int32_t x, int32_t y) const;

protected:
    struct CoveredArea {
        int32_t x1;
        int32_t y1;
        int32_t x2;
        int32_t y2;
    };

    virtual CoveredArea getCoveredArea() const = 0;
};

#endif

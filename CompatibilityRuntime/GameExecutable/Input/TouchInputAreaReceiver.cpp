#include "Input/TouchInputAreaReceiver.h"

TouchInputAreaReceiver::TouchInputAreaReceiver() = default;

TouchInputAreaReceiver::~TouchInputAreaReceiver() = default;

bool TouchInputAreaReceiver::coversPoint(int32_t x, int32_t y) const {

    auto area = getCoveredArea();

    return area.x1 <= x && area.x2 > x && area.y1 <= y && area.y2 > y;
}

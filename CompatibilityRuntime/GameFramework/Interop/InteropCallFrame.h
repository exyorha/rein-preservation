#ifndef INTEROP_INTEROP_CALL_FRAME_H
#define INTEROP_INTEROP_CALL_FRAME_H

#include <cstring>

class ARMArgumentSet;
class ARMSingleArgument;

class InteropCallFrame {
public:
    explicit InteropCallFrame(
        const ARMArgumentSet &arguments,
        const ARMSingleArgument &result,
        void **argumentSourceLocations,
        void *returnResultLocation
    );

    ~InteropCallFrame();

    InteropCallFrame(const InteropCallFrame &other) = delete;
    InteropCallFrame &operator =(const InteropCallFrame &other) = delete;

private:
    size_t m_totalStackFrameSize;
    const ARMSingleArgument &m_result;
    void *m_resultLocation;
};

#endif

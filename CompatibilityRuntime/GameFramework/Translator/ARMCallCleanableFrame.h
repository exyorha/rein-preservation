#ifndef ARM_CALL_CLEANABLE_FRAME_H
#define ARM_CALL_CLEANABLE_FRAME_H

#include <cstdint>

class JITThreadContext;

class ARMCallCleanableFrame {
public:
    explicit ARMCallCleanableFrame(JITThreadContext &context);
    ~ARMCallCleanableFrame();

    ARMCallCleanableFrame(const ARMCallCleanableFrame &other) = delete;
    ARMCallCleanableFrame &operator =(const ARMCallCleanableFrame &other) = delete;

private:
    JITThreadContext &m_context;
    uint64_t *m_frame;
};

#endif

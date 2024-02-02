#include <Translator/JITThreadContext.h>
#include <Interop/InteropCallFrame.h>
#include <Interop/ARMArgumentPacker.h>

InteropCallFrame::InteropCallFrame(
    const ARMArgumentSet &arguments,
    const ARMSingleArgument &result,
    void **argumentSourceLocations,
    void *returnResultLocation
) : m_totalStackFrameSize(arguments.argumentFrameSizeOnStack() + 16), m_result(result), m_resultLocation(returnResultLocation) {

    auto &context = JITThreadContext::get();

    /*
     * Reserve the stack frame.
     */
    context.sp -= m_totalStackFrameSize;

    auto frame = reinterpret_cast<uint64_t *>(context.sp);
    frame[0] = context.fp(); // lowest doubleword: previous frame pointer
    frame[1] = context.lr(); // next doubleword: saved LR

    context.fp() = context.sp;

    /*
     * Now, apply the arguments.
     */
    arguments.applyOntoMachineContext(argumentSourceLocations, context);
}

InteropCallFrame::~InteropCallFrame() {
    auto &context = JITThreadContext::get();

    /*
     * Capture the return.
     */
    m_result.captureFromMachineContext(m_resultLocation, context);

    auto frame = reinterpret_cast<uint64_t *>(context.sp);
    // Restore FP.
    context.fp() = frame[0];

    // Restore LR.
    context.lr() = frame[1];

    /*
     * Deallocate the stack frame.
     */
    context.sp += m_totalStackFrameSize;
}

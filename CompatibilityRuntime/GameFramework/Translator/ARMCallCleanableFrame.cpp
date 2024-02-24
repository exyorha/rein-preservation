#include <Translator/ARMCallCleanableFrame.h>
#include <Translator/JITThreadContext.h>
#include "support.h"

ARMCallCleanableFrame::ARMCallCleanableFrame(JITThreadContext &context) : m_context(context) {
    /*
     * We need to set up a call frame as detailed in bionic/call_arm_code_catching_exceptions.S::__compatibility_runtime_armcall_catcher
     */

    // stp	x29, x30, [sp, -48]!
    context.sp -= 48;
    m_frame = reinterpret_cast<uint64_t *>(context.sp);
    m_frame[0] = context.fp(); // x29 = FP
    m_frame[1] = context.pc;   // x30 = LR, we store old PC

    // mov	x29, sp
    context.fp() = context.sp;

}

ARMCallCleanableFrame::~ARMCallCleanableFrame() {
    if(m_context.fp() != reinterpret_cast<uintptr_t>(m_frame) ||
       m_context.sp != reinterpret_cast<uintptr_t>(m_frame))
        panic("ARM machine stack desynchronized with the ARMCallCleanableFrame chain");

#if 0
        /*
        * Restore sp to fp in case it was fudged with, i.e. if arguments
        * were pushed.
        */
        context.sp = context.fp();
#endif


    /*
    * Now, epilogue as described in __compatibility_runtime_armcall_catcher.
    */
    //ldp	x29, x30, [sp], 48
    m_context.fp() = m_frame[0];
    m_context.pc = m_frame[1];
    m_context.sp += 48;
}

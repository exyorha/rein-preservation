#include <Translator/WorldStoppingScope.h>
#include <Translator/JITThreadContext.h>

#include "GlobalContext.h"

WorldStoppingScope::WorldStoppingScope() {
    auto &context = JITThreadContext::get();
    m_stoppedByThis = !context.stoppedWorld;
    if(m_stoppedByThis) {
        GlobalContext::get().jit().stopWorld(context);
    }
}

WorldStoppingScope::~WorldStoppingScope() {
    if(m_stoppedByThis) {
        GlobalContext::get().jit().startWorld(JITThreadContext::get());
    }
}

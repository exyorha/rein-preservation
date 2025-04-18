#include <Translator/JITThreadContext.h>

#include <Interop/SavedICallContext.h>

#include <cstring>

SavedICallContext::SavedICallContext(const JITThreadContext &thread) {
    memcpy(&intArgs, &thread.gprs, sizeof(intArgs));
    memcpy(&vectorArgs, &thread.vectors, sizeof(vectorArgs));
    sp = thread.sp;
}

SavedICallContext::~SavedICallContext() = default;


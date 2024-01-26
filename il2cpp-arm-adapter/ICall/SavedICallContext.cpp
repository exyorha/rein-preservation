#include <Translator/JITThreadContext.h>

#include <ICall/SavedICallContext.h>

#include <cstring>

SavedICallContext::SavedICallContext(const JITThreadContext &thread) {
    memcpy(&intArgs, &thread.gprs, sizeof(intArgs));
    memcpy(&vectorArgs, &thread.vectors, sizeof(vectorArgs));
}

SavedICallContext::~SavedICallContext() = default;


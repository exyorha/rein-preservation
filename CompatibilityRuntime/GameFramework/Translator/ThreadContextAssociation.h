#ifndef TRANSLATOR_THREAD_CONTEXT_ASSOCIATION_H
#define TRANSLATOR_THREAD_CONTEXT_ASSOCIATION_H

class JITThreadContext;

JITThreadContext *getJITContextOfCurrentThread() noexcept;
void setJITContextOfCurrentThread(JITThreadContext *context) noexcept;

#endif

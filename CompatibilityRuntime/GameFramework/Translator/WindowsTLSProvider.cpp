#include <windows.h>
#include <cstdio>

#include <Translator/ThreadContextAssociation.h>
#include <Translator/JITThreadContext.h>

static DWORD ThreadContextAssociationSlot = TLS_OUT_OF_INDEXES;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if(fdwReason == DLL_PROCESS_ATTACH) {
        ThreadContextAssociationSlot = TlsAlloc();

        if(ThreadContextAssociationSlot == TLS_OUT_OF_INDEXES)
            return FALSE;

    } else if(fdwReason == DLL_PROCESS_DETACH) {
        TlsFree(ThreadContextAssociationSlot);
        ThreadContextAssociationSlot = TLS_OUT_OF_INDEXES;
    } else if(fdwReason == DLL_THREAD_ATTACH) {
        /*
         * Nothing to do here.
         */
    } else if(fdwReason == DLL_THREAD_DETACH) {

        auto context = getJITContextOfCurrentThread();

        if(context) {
            context->threadStateTeardown();

            setJITContextOfCurrentThread(nullptr);
        }
    }


    return TRUE;
}

JITThreadContext *getJITContextOfCurrentThread() noexcept {
    return static_cast<JITThreadContext *>(TlsGetValue(ThreadContextAssociationSlot));
}

void setJITContextOfCurrentThread(JITThreadContext *context) noexcept {
    if(context)
        context->addReference();

    auto existing = getJITContextOfCurrentThread();
    if(existing) {
        existing->threadStateTeardown();
        existing->release();
    }

    if(!TlsSetValue(ThreadContextAssociationSlot, context)) {
        if(context)
            context->release();
    } else {
        context->threadStateInitialization();
    }
}


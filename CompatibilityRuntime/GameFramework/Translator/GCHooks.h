#ifndef TRANSLATOR_GC_HOOKS_H
#define TRANSLATOR_GC_HOOKS_H

class Image;

void installGCHooks(const Image &il2cppImage);

#ifdef CR_GARBAGE_COLLECT_HOST_STACKS
void initializeHostGC();

void *getPlatformSpecificStackBottomForThisThread();
#else
/*
 * These functions *cannot* throw exceptions or do ARM calls.
 */
void disableARMSideGC() noexcept;
void restoreARMSideGC() noexcept;
#endif


class GCDisablingScope {
public:
    inline GCDisablingScope() {
#ifndef CR_GARBAGE_COLLECT_HOST_STACKS
        disableARMSideGC();
#endif
    }

    inline ~GCDisablingScope() {
#ifndef CR_GARBAGE_COLLECT_HOST_STACKS
        restoreARMSideGC();
#endif
    }

    GCDisablingScope(const GCDisablingScope &other) = delete;
    GCDisablingScope &operator =(const GCDisablingScope &other) = delete;
};

#endif


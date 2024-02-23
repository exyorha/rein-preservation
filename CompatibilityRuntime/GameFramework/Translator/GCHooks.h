#ifndef TRANSLATOR_GC_HOOKS_H
#define TRANSLATOR_GC_HOOKS_H

class Image;

void initializeHostGC();
void installGCHooks(const Image &il2cppImage);

void *getPlatformSpecificStackBottomForThisThread();

#endif


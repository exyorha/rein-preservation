#ifndef GLES_WGL_WGL_HOOKING_H
#define GLES_WGL_WGL_HOOKING_H

#include <GLES/GLESImplementationType.h>

void replaceUnityWGL(GLESImplementationType implementationType);

void *getWGLProc(const char *name);
void *getExtensionWGLProc(const char *name);

#endif

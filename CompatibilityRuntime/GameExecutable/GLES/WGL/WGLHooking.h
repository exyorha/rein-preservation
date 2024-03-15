#ifndef GLES_WGL_WGL_HOOKING_H
#define GLES_WGL_WGL_HOOKING_H

void replaceUnityWGL();

void *getWGLProc(const char *name);
void *getExtensionWGLProc(const char *name);

#endif

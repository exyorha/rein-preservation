#ifndef GLES_WGL_WINDOWS_IMPORT_REDIRECTION_H
#define GLES_WGL_WINDOWS_IMPORT_REDIRECTION_H

#include <windows.h>
#include <cstring>

struct ReplacementExport {
    const char *name;
    void *address;
    void **originalAddress;
};

const ReplacementExport *findEntryInReplacementExports(
    const char *name, const ReplacementExport *rebindTo, size_t numberOfReboundExports);

void rebindModuleImport(HMODULE importingModule, const IMAGE_IMPORT_DESCRIPTOR *importedModule,
                        const ReplacementExport *rebindTo, size_t numberOfReboundExports, bool ignoreOthers = false);

void rebindModuleImport(HMODULE importingModule, const char *importedModule, const ReplacementExport *rebindTo,
                        size_t numberOfReboundExports, bool ignoreOthers = false);

#endif

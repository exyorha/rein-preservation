#include <GLES/WGL/WGLHooking.h>
#include <GLES/WGL/WindowsImportRedirection.h>
#include <GLES/GLESRenderingOverlay.h>

#include <stdexcept>

static WINBOOL WINAPI replacement_SwapBuffers(HDC hdc);

static WINBOOL WINAPI (*original_SwapBuffers)(HDC hdc);

static const ReplacementExport replacementGDIFunctions[]{
    { "SwapBuffers", reinterpret_cast<void *>(replacement_SwapBuffers), reinterpret_cast<void **>(&original_SwapBuffers) }
};

void installWGLOverlayHook() {
    auto unityModule = GetModuleHandle(L"UnityPlayer.dll");
    if(!unityModule) {
        throw std::runtime_error("Unable to find UnityPlayer.dll");
    }

    rebindModuleImport(unityModule, "gdi32.dll", replacementGDIFunctions,
                    sizeof(replacementGDIFunctions) / sizeof(replacementGDIFunctions[0]), true);
}

WINBOOL WINAPI replacement_SwapBuffers(HDC hdc) {

    RECT rect;

    GetClientRect(WindowFromDC(hdc), &rect);

    GLESRenderingOverlay::invokeBeforeSwapBuffers(rect.right, rect.bottom);

    auto result = original_SwapBuffers(hdc);

    GLESRenderingOverlay::invokeAfterSwapBuffers();

    return result;
}

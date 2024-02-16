#ifndef GLES_SDL_SDL_GLES_IMPLEMENTATION_NATIVE_H
#define GLES_SDL_SDL_GLES_IMPLEMENTATION_NATIVE_H

#include <GLES/BaseGLESContext.h>
#include <GLES/SDL/SDLGLESImplementation.h>

class SDLGLESImplementationNative final : public SDLGLESImplementation {
public:
    SDLGLESImplementationNative();
    ~SDLGLESImplementationNative();

    SDL_Window *CreateWindow(const char *title, int x, int y, int w, int h, Uint32 flags) noexcept override;
protected:
    void DestroyWindowImpl(SDL_Window *window) noexcept override;

    std::unique_ptr<BaseGLESContext> CreateContextImpl(SDL_Window *window) override;

public:
    SDL_bool ExtensionSupported(const char *extension) noexcept override;

protected:
    int MakeCurrentImpl(SDL_Window *window, SDL_GLContext context) noexcept override;

public:
    int GetAttribute(SDL_GLattr attr, int *value) noexcept override;
    void ResetAttributes() noexcept override;
    int SetAttribute(SDL_GLattr attr, int value) noexcept override;

    int GetSwapInterval() noexcept override;
    int SetSwapInterval(int interval) noexcept override;
    void SwapWindow(SDL_Window *window) noexcept override;

    int LoadLibrary(const char *path) noexcept override;
    void UnloadLibrary() noexcept override;

private:
    class NativeWrapperContext final : public BaseGLESContext {
    public:
        explicit NativeWrapperContext(SDL_GLContext nativeContext);
        ~NativeWrapperContext() override;

        inline SDL_GLContext context() const {
            return m_context;
        }

        static inline SDL_GLContext unwrap(SDL_GLContext handle) {
            if(!handle)
                return nullptr;

            return static_cast<NativeWrapperContext *>(handle)->context();
        }

        void *getProcAddress(const char *name) noexcept override;

    private:
        SDL_GLContext m_context;
    };

};

#endif

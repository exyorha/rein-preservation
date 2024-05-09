#ifndef WEB_VIEW_RENDER_HANDLER_H
#define WEB_VIEW_RENDER_HANDLER_H

#include <include/cef_render_handler.h>

#include <memory>

class WebViewSharedImageBuffer;

class WebViewRenderHandler final : public CefRenderHandler {
private:

    IMPLEMENT_REFCOUNTING(WebViewRenderHandler);

    ~WebViewRenderHandler();

public:
    WebViewRenderHandler(const CefRect &rect, std::unique_ptr<WebViewSharedImageBuffer> &&sharedMemory);

    inline const CefRect &rect() const {
        return m_rect;
    }

    inline void setRect(const CefRect &rect) {
        m_rect = rect;
    }

    void replaceSharedMemoryRegion(std::unique_ptr<WebViewSharedImageBuffer> &&sharedMemory);

    void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;

    void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type,
                 const RectList& dirtyRects, const void* buffer, int width, int height) override;

private:
    CefRect m_rect;
    std::unique_ptr<WebViewSharedImageBuffer> m_sharedMemory;
};

#endif

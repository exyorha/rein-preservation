#include "WebViewRenderHandler.h"
#include "WebViewSharedImageBuffer.h"

WebViewRenderHandler::WebViewRenderHandler(const CefRect &rect, std::unique_ptr<WebViewSharedImageBuffer> &&sharedMemory) :
    m_rect(rect), m_sharedMemory(std::move(sharedMemory)) {

}

WebViewRenderHandler::~WebViewRenderHandler() = default;

void WebViewRenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) {
    (void)browser;
    rect = m_rect;
}

void WebViewRenderHandler::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type,
                                   const RectList& dirtyRects, const void* buffer, int width, int height) {

    printf("WebViewRenderHandler: type %d buffer %p width %d height %d shared memory %p\n",
           type, buffer, width, height, m_sharedMemory.get());

    if(!m_sharedMemory || width <= 0 || height <= 0)
        return;

    auto imageData = m_sharedMemory->imageData();
    auto pitch = m_sharedMemory->pitch();

    auto dstWidth = m_sharedMemory->width();
    auto dstHeight = m_sharedMemory->height();

    auto srcPitch = width * sizeof(uint32_t);

    for(const auto &rect: dirtyRects) {
        auto x1 = rect.x;
        auto y1 = rect.y;
        auto x2 = rect.x + rect.width;
        auto y2 = rect.y + rect.height;

        x1 = std::max(0, x1);
        y1 = std::max(0, y1);
        x2 = std::min<int>(dstWidth, x2);
        y2 = std::min<int>(dstHeight, y2);
        x2 = std::min<int>(width, x2);
        y2 = std::min<int>(height, y2);

        printf("painting rect: %d, %d, %d, %d\n", x1, y1, x2, y2);

        if(x2 <= x1 || y2 <= y1)
            continue;

        for(int row = y1; row < y2; row++) {
            memcpy(
                imageData + row * pitch + x1 * sizeof(uint32_t),
                static_cast<const uint8_t *>(buffer) + row * srcPitch + x1 * sizeof(uint32_t),
                (x2 - x1) * sizeof(uint32_t));
        }
    }

    m_sharedMemory->clearCleanFlag();
}

void WebViewRenderHandler::replaceSharedMemoryRegion(std::unique_ptr<WebViewSharedImageBuffer> &&sharedMemory) {
    m_sharedMemory = std::move(sharedMemory);
}

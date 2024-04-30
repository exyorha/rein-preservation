#include <WebView/CEFSurface.h>

#include <WebViewSharedImageBuffer.h>

CEFSurface::CEFSurface(int32_t x, int32_t y, uint32_t width, uint32_t height,
                       std::unique_ptr<WebViewSharedImageBuffer> &&buffer) :
                       m_buffer(std::move(buffer)),
                       m_x(x), m_y(y), m_width(width), m_height(height), m_hidden(false) {

    m_buffer->initializeHeader(m_width, m_height);

}

CEFSurface::~CEFSurface() = default;

void CEFSurface::setPosition(int32_t x, int32_t y) {
    m_x = x;
    m_y = y;
}

void CEFSurface::setSize(uint32_t width, uint32_t height, std::unique_ptr<WebViewSharedImageBuffer> &&buffer) {
    m_buffer = std::move(buffer);
    m_width = width;
    m_height = height;
    m_buffer->initializeHeader(m_width, m_height);
}

void CEFSurface::setFrame(int32_t x, int32_t y, uint32_t width, uint32_t height,
                std::unique_ptr<WebViewSharedImageBuffer> &&buffer) {

    m_buffer = std::move(buffer);
    m_x = x;
    m_y = y;
    m_width = width;
    m_height = height;
    m_buffer->initializeHeader(m_width, m_height);
}

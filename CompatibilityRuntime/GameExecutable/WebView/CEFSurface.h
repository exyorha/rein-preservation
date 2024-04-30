#ifndef WEBVIEW_CEF_SURFACE_H
#define WEBVIEW_CEF_SURFACE_H

#include <cstdint>
#include <cstring>
#include <memory>

class WebViewSharedImageBuffer;

class CEFSurface {
public:
    CEFSurface(int32_t x, int32_t y, uint32_t width, uint32_t height,
               std::unique_ptr<WebViewSharedImageBuffer> &&buffer);
    ~CEFSurface();

    CEFSurface(const CEFSurface &other) = delete;
    CEFSurface &operator =(const CEFSurface &other) = delete;

    inline bool hidden() const {
        return m_hidden;
    }

    inline void setHidden(bool hidden) {
        m_hidden = hidden;
    }

    inline int32_t x() const {
        return m_x;
    }

    inline int32_t y() const {
        return m_y;
    }

    inline uint32_t width() const {
        return m_width;
    }

    inline uint32_t height() const {
        return m_height;
    }

    void setPosition(int32_t x, int32_t y);

    void setSize(uint32_t width, uint32_t height, std::unique_ptr<WebViewSharedImageBuffer> &&buffer);

    void setFrame(int32_t x, int32_t y, uint32_t width, uint32_t height,
                  std::unique_ptr<WebViewSharedImageBuffer> &&buffer);

private:
    std::unique_ptr<WebViewSharedImageBuffer> m_buffer;
    int32_t m_x;
    int32_t m_y;
    uint32_t m_width;
    uint32_t m_height;
    bool m_hidden;
};

#endif

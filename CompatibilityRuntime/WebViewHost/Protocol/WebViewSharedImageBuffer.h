#ifndef WEBVIEW_SHARED_IMAGE_BUFFER_H
#define WEBVIEW_SHARED_IMAGE_BUFFER_H

#include <cstring>
#include <cstdint>

struct WebViewSharedImageBufferHeader {
    uint32_t width;
    uint32_t height;
    uint8_t pad[8];
};

static_assert(sizeof(WebViewSharedImageBufferHeader) == 16, "WebViewSharedImageBufferHeader is expected to be 16 byte long");

class WebViewSharedImageBuffer {
protected:
    WebViewSharedImageBuffer() = default;

public:
    virtual ~WebViewSharedImageBuffer() = default;

    WebViewSharedImageBuffer(const WebViewSharedImageBuffer &other) = delete;
    WebViewSharedImageBuffer &operator =(const WebViewSharedImageBuffer &other) = delete;

    virtual void *base() const = 0;
    virtual size_t size() const = 0;

    void initializeHeader(uint32_t width, uint32_t height) {
        auto header = imageHeader();
        memset(header, 0, sizeof(*header));
        header->width = width;
        header->height = height;
    }

    inline uint32_t width() const {
        return imageHeader()->width;
    }

    inline uint32_t height() const {
        return imageHeader()->height;
    }

    inline unsigned char *imageData() const {
        return static_cast<unsigned char *>(base()) + sizeof(WebViewSharedImageBufferHeader);
    }

    inline size_t pitch() const {
        return width() * sizeof(uint32_t);
    }

    static inline size_t sizeBuffer(uint32_t width, uint32_t height) {
        return
            sizeof(WebViewSharedImageBufferHeader) +
            static_cast<size_t>(width) * static_cast<size_t>(height) * sizeof(uint32_t);
    }

private:
    inline WebViewSharedImageBufferHeader *imageHeader() {
        return static_cast<WebViewSharedImageBufferHeader *>(base());
    }

    inline const WebViewSharedImageBufferHeader *imageHeader() const {
        return static_cast<const WebViewSharedImageBufferHeader *>(base());
    }
};

#endif


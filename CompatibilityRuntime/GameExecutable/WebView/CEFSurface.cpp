#include <WebView/CEFSurface.h>
#include <WebView/CEFCompositor.h>

#include <WebViewSharedImageBuffer.h>

CEFSurface::CEFSurface(int32_t x, int32_t y, uint32_t width, uint32_t height,
                       std::unique_ptr<WebViewSharedImageBuffer> &&buffer) :
                       m_buffer(std::move(buffer)),
                       m_x(x), m_y(y), m_width(width), m_height(height), m_hidden(false),
                       m_compositor(nullptr), m_allocatedWidth(0), m_allocatedHeight(0) {

    m_buffer->initializeHeader(m_width, m_height);

}

CEFSurface::~CEFSurface() {
    if(m_compositor && m_texture) {
        m_compositor->scheduleTextureDisposal(std::move(m_texture));
    }
}

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

bool CEFSurface::shouldRender() const {
    return !m_hidden && m_buffer;
}

bool CEFSurface::getAndClearNeedsUploadFlag() {
    if(!m_buffer)
        return false;

    return !m_buffer->setAndReturnCleanFlag();
}

void CEFSurface::upload(CEFCompositor *compositor) {


    const auto &api = GLESAPISet::get();

    GLint oldTexture;
    api.glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldTexture);

    if(!m_texture || m_allocatedWidth != m_width || m_allocatedHeight != m_height) {

        printf("CEFSurface: recreating %p\n", this);

        m_texture = GLESTextureHandle::create();
        m_compositor = compositor;

        api.glBindTexture(GL_TEXTURE_2D, m_texture);
        api.glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        api.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        api.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        api.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        api.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        api.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        api.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        m_texture.label("CEFSurface " + std::to_string(reinterpret_cast<uintptr_t>(this)));

        m_allocatedWidth = m_width;
        m_allocatedHeight = m_height;
    } else {

        api.glBindTexture(GL_TEXTURE_2D, m_texture);
    }


    printf("CEFSurface: updating the pixel data for %p\n", this);

    api.glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_buffer->width(), m_buffer->height(), GL_RGBA, GL_UNSIGNED_BYTE,
                        m_buffer->imageData());

    api.glBindTexture(GL_TEXTURE_2D, oldTexture);
}

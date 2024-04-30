#include "WebViewRPCServerLinux.h"

#include <fcntl.h>

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <include/wrapper/cef_closure_task.h>
#include <include/base/cef_callback.h>
#include <include/cef_app.h>

WebViewRPCServerLinux::WebViewRPCServerLinux(intptr_t connectedSocket) : m_socket(connectedSocket),
    m_receiveBuffer(256 * 1024), m_controlBuffer(4096), m_fds(nullptr) {

    printf("WebViewRPCServerLinux: working over %d\n", m_socket);

    fcntl(m_socket, F_SETFD, fcntl(m_socket, F_GETFD) | FD_CLOEXEC);
    m_requestReceivingThread = std::thread(&WebViewRPCServerLinux::receiveRequests, this);
}

WebViewRPCServerLinux::~WebViewRPCServerLinux() {
    shutdown(m_socket, SHUT_RDWR);

    clearCallInProgress();

    m_requestReceivingThread.join();
}

void WebViewRPCServerLinux::receiveRequests() {
    printf("WebViewRPCServerLinux: receiving requests\n");

    runMainLoop();

    printf("WebViewRPCServerLinux: main loop exited, signaling the browser UI thread to stop\n");

    CefPostTask(TID_UI, base::BindOnce(CefQuitMessageLoop));
}

void WebViewRPCServerLinux::executeRPCCall(std::unique_ptr<webview::protocol::RPCRequest> &&request) {
    if(!executeRPCCallAndSendResponse(std::move(request))) {

        fprintf(stderr, "WebViewRPCServerLinux: an error occurred during RPC processing\n");

        CefQuitMessageLoop();
    }
}

bool WebViewRPCServerLinux::executeRPCCallAndSendResponse(std::unique_ptr<webview::protocol::RPCRequest> &&request) {
    auto result = doExecuteRPCCall(std::move(request));
    if(!result.has_value()) {
        return false;
    }

    const auto &fullResponseData = *result;

    auto sent = send(m_socket, fullResponseData.data(), fullResponseData.size(), MSG_EOR | MSG_NOSIGNAL);
    if(sent < 0 || static_cast<size_t>(sent) != fullResponseData.size())
        return false;

    clearCallInProgress();

    return true;
}

void WebViewRPCServerLinux::runMainLoop() {
    while(true) {
        waitNoCallInProgress();

        if(m_fds) {
            printf("WebViewRPCServerLinux: releasing the previously received fds\n");

            for(auto ptr = reinterpret_cast<const unsigned char *>(CMSG_DATA(m_fds)),
                limit = ptr + m_fds->cmsg_len - sizeof(cmsghdr);
                ptr < limit;
                ptr += sizeof(int)) {

                int fdval;
                memcpy(&fdval, ptr, sizeof(fdval));
                close(fdval);
            }

            m_fds = nullptr;
        }

        struct iovec iov;
        iov.iov_base = m_receiveBuffer.data();
        iov.iov_len = m_receiveBuffer.size();

        struct msghdr message;
        memset(&message, 0, sizeof(message));
        message.msg_iov = &iov;
        message.msg_iovlen = 1;
        message.msg_control = m_controlBuffer.data();
        message.msg_controllen = m_controlBuffer.size();

        auto messageLength = recvmsg(m_socket, &message, 0);

        if(messageLength < 0) {
            return;
        }

        if(message.msg_flags & (MSG_TRUNC | MSG_CTRUNC)) {
            fprintf(stderr, "WebViewRPCServerLinux: overly long incoming message\n");
            return;
        }

        for(auto hdr = CMSG_FIRSTHDR(&message); hdr; hdr = CMSG_NXTHDR(&message, hdr)) {
            if(hdr->cmsg_level == SOL_SOCKET && hdr->cmsg_type == SCM_RIGHTS) {
                m_fds = hdr;
                printf("WebViewRPCServerLinux: received %zu bytes of SCM_RIGHTS\n", m_fds->cmsg_len - sizeof(cmsghdr));
                break;
            }
        }

        auto fullRequest = std::make_unique<webview::protocol::RPCRequest>();
        if(!fullRequest->ParseFromArray(m_receiveBuffer.data(), messageLength)) {
            fprintf(stderr, "WebViewRPCServer: failed to parse the RPC request\n");
            return;
        }

        setCallInProgress();

        CefPostTask(TID_UI, base::BindOnce(&WebViewRPCServerLinux::executeRPCCall, base::Unretained(this), std::move(fullRequest)));
    }
}

std::unique_ptr<WebViewSharedImageBuffer> WebViewRPCServerLinux::receiveImageBuffer(intptr_t handle) {
    if(handle == 0)
        return nullptr;

    if(!m_fds || handle < 0 || static_cast<size_t>(handle) - 1 >= (m_fds->cmsg_len - sizeof(cmsghdr)) / sizeof(int))
        throw std::runtime_error("bad image buffer handle");

    int fd;
    memcpy(&fd, reinterpret_cast<const unsigned char *>(CMSG_DATA(m_fds)) + sizeof(int) * (handle - 1), sizeof(fd));

    return std::make_unique<LinuxSharedImageBuffer>(fd);
}

WebViewRPCServerLinux::LinuxSharedImageBuffer::LinuxSharedImageBuffer(int fd) {

    struct stat st;
    if(fstat(fd, &st) < 0) {
        throw std::runtime_error("fstat has failed");
    }

    m_size = st.st_size;
    m_base = mmap(nullptr, m_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(m_base == MAP_FAILED)
        throw std::runtime_error("mmap has failed");
}

WebViewRPCServerLinux::LinuxSharedImageBuffer::~LinuxSharedImageBuffer() {
    munmap(m_base, m_size);
}

void *WebViewRPCServerLinux::LinuxSharedImageBuffer::base() const {
    return m_base;
}

size_t WebViewRPCServerLinux::LinuxSharedImageBuffer::size() const {
    return m_size;
}

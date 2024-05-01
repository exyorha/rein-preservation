#include <WebViewHostClientChannelLinux.h>
#include <WebViewHostClientConfiguration.h>

#include <WebView/WebViewProtocol.pb.h>

#include <system_error>

#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <spawn.h>
#include <sys/wait.h>
#include <sys/mman.h>

WebViewHostClientChannelLinux::WebViewHostClientChannelLinux(const WebViewHostClientConfiguration &config) :
    m_receiveBuffer(256 * 1024) {

    std::optional<FileDescriptor> clientSideDescriptor;
    std::optional<FileDescriptor> browserSideDescriptor;

    {

        int commSockets[2];
        if(socketpair(AF_UNIX, SOCK_SEQPACKET, 0, commSockets) < 0) {
            throw std::system_error(errno, std::generic_category());
        }

        clientSideDescriptor.emplace(commSockets[0]);
        browserSideDescriptor.emplace(commSockets[1]);
    }

    int flags = fcntl(*clientSideDescriptor, F_GETFD);
    if(flags < 0)
        throw std::system_error(errno, std::generic_category());

    if(fcntl(*clientSideDescriptor, F_SETFD, flags | FD_CLOEXEC) < 0)
        throw std::system_error(errno, std::generic_category());

    {
        std::vector<std::string> hostInvocation;
        hostInvocation.emplace_back(config.hostExecutablePath.string());
        hostInvocation.emplace_back("--webview-rpc-client-socket=" + std::to_string(*browserSideDescriptor));
        hostInvocation.emplace_back("--disable-gpu");
        hostInvocation.emplace_back("--disable-gpu-compositing");
        hostInvocation.emplace_back("--disable-software-rasterizer");
        hostInvocation.emplace_back("--in-process-gpu");

        if(!config.homePath.empty()) {
            hostInvocation.emplace_back("--home-path=" + config.homePath.string());
        }

        std::vector<char *> pointers;
        pointers.reserve(hostInvocation.size() + 1);

        for(auto &string: hostInvocation) {
            pointers.emplace_back(string.data());
        }

        pointers.emplace_back(nullptr);

        pid_t pid;
        auto result = posix_spawn(&m_pid, pointers[0], nullptr, nullptr, pointers.data(), environ);

        browserSideDescriptor.reset();

        if(result != 0) {
            throw std::system_error(result, std::generic_category());
        }
    }

    m_fd = std::move(*clientSideDescriptor);
}

WebViewHostClientChannelLinux::~WebViewHostClientChannelLinux() {
    m_fd = FileDescriptor();

    /*
     * Wait for the browser process to exit.
     */
    int status;

    pid_t pid;
    do {
        pid = waitpid(m_pid, &status, 0);
    } while(pid < 0 && errno == EINTR);
}

WebViewHostClientChannelLinux::FileDescriptor::~FileDescriptor() {
    if(m_fd >= 0)
        close(m_fd);
}

WebViewHostClientChannelLinux::FileDescriptor::FileDescriptor(FileDescriptor &&other) noexcept : FileDescriptor() {
    *this = std::move(other);
}

auto WebViewHostClientChannelLinux::FileDescriptor::operator =(FileDescriptor &&other) noexcept -> FileDescriptor & {
    if(this != &other) {
        if(m_fd >= 0) {
            close(m_fd);
            m_fd = -1;
        }

        m_fd = other.m_fd;
        other.m_fd = -1;
    }

    return *this;
}

void WebViewHostClientChannelLinux::CallMethod(
    const google::protobuf::MethodDescriptor* method,
    google::protobuf::RpcController* controller,
    const google::protobuf::Message* request,
    google::protobuf::Message* response,
    google::protobuf::Closure* done) {

    controller->Reset();

    {
        webview::protocol::RPCRequest fullRequest;
        fullRequest.set_method(method->name());
        if(request)
            fullRequest.set_request(request->SerializeAsString());

        auto requestData = fullRequest.SerializeAsString();

        std::vector<unsigned char> control;

        if(!m_pendingFileDescriptors.empty()) {
            control.resize(CMSG_SPACE(m_pendingFileDescriptors.size() * sizeof(int)));
        }

        struct iovec iov;
        iov.iov_base = requestData.data();
        iov.iov_len = requestData.size();

        struct msghdr message;
        memset(&message, 0, sizeof(message));
        message.msg_iov = &iov;
        message.msg_iovlen = 1;
        message.msg_control = control.data();
        message.msg_controllen = control.size();

        if(!m_pendingFileDescriptors.empty()) {
            auto hdr = CMSG_FIRSTHDR(&message);
            hdr->cmsg_level = SOL_SOCKET;
            hdr->cmsg_type = SCM_RIGHTS;
            hdr->cmsg_len = CMSG_LEN(m_pendingFileDescriptors.size() * sizeof(int));

            auto fdp = reinterpret_cast<unsigned char *>(CMSG_DATA(hdr));
            for(const auto &fd: m_pendingFileDescriptors) {
                int fdValue = fd;
                memcpy(fdp, &fdValue, sizeof(fdValue));
                fdp += sizeof(fdValue);
            }
        }

        auto result = sendmsg(m_fd, &message, MSG_EOR | MSG_NOSIGNAL);
        m_pendingFileDescriptors.clear();

        if(result < 0)
            throw std::runtime_error("failed to send the request");

        if(result != requestData.size())
            throw std::runtime_error("not the whole message was sent");
    }

    auto responseLength = recv(m_fd, m_receiveBuffer.data(), m_receiveBuffer.size(), 0);
    if(responseLength < 0)
        throw std::runtime_error("failed to receive the response");

    webview::protocol::RPCResponse fullResponse;
    if(!fullResponse.ParseFromArray(m_receiveBuffer.data(), responseLength))
        throw std::runtime_error("failed to parse the RPC response");

    if(fullResponse.has_errormessage()) {
        controller->SetFailed(fullResponse.errormessage());
    }

    if(response) {
        if(fullResponse.has_response()) {
            if(!response->ParseFromString(fullResponse.response()))
                throw std::runtime_error("failed to parse the RPC response body");
        } else {
            response->Clear();
        }
    }

    if(done)
        done->Run();
}

std::unique_ptr<WebViewSharedImageBuffer> WebViewHostClientChannelLinux::allocateImageBuffer(size_t size) {
    return std::make_unique<LinuxImageBuffer>(size);
}

WebViewHostClientChannelLinux::LinuxImageBuffer::LinuxImageBuffer(size_t size) : m_base(nullptr), m_size(size) {
    if(size == 0)
        throw std::logic_error("the size cannot be zero");

    auto rawfd = memfd_create("WebViewHostClientChannelLinux_ImageBuffer", MFD_CLOEXEC | MFD_ALLOW_SEALING);
    if(rawfd < 0)
        throw std::system_error(errno, std::generic_category());

    m_fd = FileDescriptor(rawfd);

    if(ftruncate64(m_fd, size) < 0)
        throw std::system_error(errno, std::generic_category());

    if(fcntl(m_fd, F_ADD_SEALS, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW) < 0)
        throw std::system_error(errno, std::generic_category());

    m_base = mmap(nullptr, m_size, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0);
    if(m_base == MAP_FAILED)
        throw std::system_error(errno, std::generic_category());
}

WebViewHostClientChannelLinux::LinuxImageBuffer::~LinuxImageBuffer() {
    munmap(m_base, m_size);
}

void *WebViewHostClientChannelLinux::LinuxImageBuffer::base() const {
    return m_base;
}

size_t WebViewHostClientChannelLinux::LinuxImageBuffer::size() const {
    return m_size;
}

/*
 * For linux, the image buffer handle encoding is:
 * 0 - 'no valid FD'
 * > 0 - the FD in the SCM_RIGHTS array at the index of (handle - 1).
 */
int64_t WebViewHostClientChannelLinux::sendSharedImageBufferWithNextRequest(WebViewSharedImageBuffer *buffer) {
    if(buffer == nullptr)
        return 0;

    auto newFd = fcntl(static_cast<LinuxImageBuffer *>(buffer)->fd(), F_DUPFD_CLOEXEC);
    if(newFd < 0)
        throw std::runtime_error("fcntl(F_DUPFD_CLOEXEC) has failed");

    FileDescriptor owned(newFd);

    m_pendingFileDescriptors.emplace_back(std::move(owned));

    return m_pendingFileDescriptors.size();
}

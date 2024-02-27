#include <WebViewHostClient.h>

#include <vector>
#include <string>
#include <optional>
#include <system_error>

#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <spawn.h>
#include <sys/wait.h>

WebViewHostClient::WebViewHostClient(const std::filesystem::path &executableDirectory) {
    std::optional<FileDescriptor> clientSideDescriptor;
    std::optional<FileDescriptor> browserSideDescriptor;

    {

        int commSockets[2];
        if(socketpair(AF_UNIX, SOCK_STREAM, 0, commSockets) < 0) {
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
        hostInvocation.emplace_back((executableDirectory / "webview" / "linux" / "WebViewHost").string());
        hostInvocation.emplace_back("--webview-rpc-client-socket=" + std::to_string(*browserSideDescriptor));

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

    m_channel.emplace(std::move(*clientSideDescriptor));
    m_stub.emplace(&*m_channel);
}

WebViewHostClient::~WebViewHostClient() {
    /*
     * Wait for the browser process to exit.
     */
    int status;

    pid_t pid;
    do {
        pid = waitpid(m_pid, &status, 0);
    } while(pid < 0 && errno == EINTR);
}

WebViewHostClient::FileDescriptor::~FileDescriptor() {
    if(m_fd >= 0)
        close(m_fd);
}

WebViewHostClient::FileDescriptor::FileDescriptor(FileDescriptor &&other) noexcept : FileDescriptor() {
    *this = std::move(other);
}

auto WebViewHostClient::FileDescriptor::operator =(FileDescriptor &&other) noexcept -> FileDescriptor & {
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

WebViewHostClient::Channel::Channel(FileDescriptor &&fd) : m_fd(std::move(fd)) {

}

WebViewHostClient::Channel::~Channel() = default;


void WebViewHostClient::Channel::CallMethod(
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
        uint32_t requestLength = requestData.size();

        if(!ensuredWrite(&requestLength, sizeof(requestLength)))
            throw std::runtime_error("failed to write the request header");

        if(!ensuredWrite(requestData.data(), requestData.size()))
            throw std::runtime_error("failed to write the request body");
    }

    uint32_t responseLength;
    if(!ensuredRead(&responseLength, sizeof(responseLength)))
        throw std::runtime_error("failed to read the response header");

    if(m_receiveBuffer.size() < responseLength)
        m_receiveBuffer.resize(responseLength);

    if(!ensuredRead(m_receiveBuffer.data(), responseLength))
        throw std::runtime_error("failed to read the response header");

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

bool WebViewHostClient::Channel::ensuredWrite(const void *dest, size_t size) {
    if(size == 0)
        return true;

    auto bytes = static_cast<const unsigned char *>(dest);
    while(size > 0) {
        ssize_t result;
        do {
            result = send(m_fd, bytes, size, 0);
        } while(result < 0 && errno == EINTR);

        if(result <= 0) {
            fprintf(stderr, "WebViewHostClient::Channel::ensuredRead: send signaled %s\n", strerror(errno));
            return false;
        }

        bytes += result;
        size -= result;
    }

    return true;
}

bool WebViewHostClient::Channel::ensuredRead(void *dest, size_t size) {
    if(size == 0)
        return true;

    auto bytes = static_cast<unsigned char *>(dest);
    while(size > 0) {
        ssize_t result;
        do {
            result = recv(m_fd, bytes, size, 0);
        } while(result < 0 && errno == EINTR);

        if(result < 0) {
            fprintf(stderr, "WebViewHostClient::Channel::ensuredRead: recv signaled %s\n", strerror(errno));
            return false;
        } else if(result == 0) {
            fprintf(stderr, "WebViewHostClient::Channel::ensuredRead: end of file\n");
            return false;
        }

        bytes += result;
        size -= result;
    }

    return true;
}


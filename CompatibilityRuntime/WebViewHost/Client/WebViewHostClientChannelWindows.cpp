#include <WebViewHostClientChannelWindows.h>
#include <WebViewHostClientConfiguration.h>

#include <windows.h>

#include <WebView/WebViewProtocol.pb.h>

WebViewHostClientChannelWindows::WebViewHostClientChannelWindows(const WebViewHostClientConfiguration &config) :
    m_receiveBuffer(128 * 1024) {
    std::wstringstream pipeNameStream;
    pipeNameStream << L"\\\\.\\pipe\\LOCAL\\WebViewHostClientChannel-" << GetCurrentProcessId() << "-" << reinterpret_cast<uintptr_t>(this);

    auto pipeName = pipeNameStream.str();

    printf("WebViewHostClientChannelWindows: will use %ls for communication with the WebView process\n",
           pipeName.c_str());

    auto rawHandle = CreateNamedPipe(
        pipeName.c_str(),
        PIPE_ACCESS_DUPLEX | FILE_FLAG_FIRST_PIPE_INSTANCE,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT | PIPE_REJECT_REMOTE_CLIENTS,
        1, 0, 0, 1000, nullptr);
    if(rawHandle == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("CreatePipe has failed");
    }

    m_handle = HandleHolder(rawHandle);

    std::wstringstream cmdline;
    cmdline << L"\"" << config.hostExecutablePath.wstring() << L"\" \"--webview-rpc-server=" << pipeName << L"\"";

    if(!config.homePath.empty()) {
        cmdline << L" \"--home-path=" << config.homePath << L"\"";
    }

    cmdline << L" --disable-gpu --disable-gpu-compositing --disable-software-rasterizer --in-process-gpu --enable-begin-frame-scheduling";

    STARTUPINFO si;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi;

    if(!CreateProcess(
        config.hostExecutablePath.wstring().c_str(),
        cmdline.str().data(),
        nullptr,
        nullptr,
        FALSE,
        CREATE_DEFAULT_ERROR_MODE,
        nullptr,
        nullptr,
        &si,
        &pi))
        throw std::runtime_error("CreateProcess has failed");

    m_process = HandleHolder(pi.hProcess);
    CloseHandle(pi.hThread);

    printf("WebViewHostClientChannelWindows: created process %u, waiting for it to connect\n",
           pi.dwProcessId);

    if(!ConnectNamedPipe(m_handle, nullptr))
        throw std::runtime_error("ConnectNamedPipe has failed");

    printf("WebViewHostClientChannelWindows: process connected\n");

}

WebViewHostClientChannelWindows::~WebViewHostClientChannelWindows() {
    m_handle = HandleHolder();

    if(m_process) {

        /*
         * Wait for the browser process to exit.
         */

        WaitForSingleObject(m_process, INFINITE);
    }
}

WebViewHostClientChannelWindows::HandleHolder::~HandleHolder() {
    if(m_handle)
        CloseHandle(m_handle);
}

WebViewHostClientChannelWindows::HandleHolder::HandleHolder(HandleHolder &&other) noexcept : HandleHolder() {
    *this = std::move(other);
}

auto WebViewHostClientChannelWindows::HandleHolder::operator =(HandleHolder &&other) noexcept -> HandleHolder & {
    if(this != &other) {
        if(m_handle) {
            CloseHandle(m_handle);
            m_handle = nullptr;
        }

        m_handle = other.m_handle;
        other.m_handle = nullptr;
    }

    return *this;
}

void WebViewHostClientChannelWindows::CallMethod(
    const google::protobuf::MethodDescriptor* method,
    google::protobuf::RpcController* controller,
    const google::protobuf::Message* request,
    google::protobuf::Message* response,
    google::protobuf::Closure* done) {

    controller->Reset();

    std::string requestData;

    {
        webview::protocol::RPCRequest fullRequest;
        fullRequest.set_method(method->name());
        if(request)
            fullRequest.set_request(request->SerializeAsString());

        requestData = fullRequest.SerializeAsString();
    }

    DWORD bytesRead;
    if(!TransactNamedPipe(m_handle,
                      requestData.data(), requestData.size(),
                      m_receiveBuffer.data(), m_receiveBuffer.size(), &bytesRead,
                      nullptr))
        throw std::runtime_error("TransactNamedPipe has failed");

    webview::protocol::RPCResponse fullResponse;
    if(!fullResponse.ParseFromArray(m_receiveBuffer.data(), bytesRead))
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


std::unique_ptr<WebViewSharedImageBuffer> WebViewHostClientChannelWindows::allocateImageBuffer(size_t size) {
    return std::make_unique<WindowsImageBuffer>(size);
}

WebViewHostClientChannelWindows::WindowsImageBuffer::WindowsImageBuffer(size_t size) : m_base(nullptr), m_size(size) {
    if(size == 0)
        throw std::runtime_error("the buffer size cannot be zero");

    auto rawhandle = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE,
                                       static_cast<DWORD>(size >> 32),
                                       static_cast<DWORD>(size),
                                       nullptr);
    if(!rawhandle)
        throw std::runtime_error("CreateFileMapping has failed");

    m_handle = HandleHolder(rawhandle);

    m_base = MapViewOfFile(m_handle, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
    if(m_base == nullptr)
        throw std::runtime_error("MapViewOfFile has failed");
}

WebViewHostClientChannelWindows::WindowsImageBuffer::~WindowsImageBuffer() {
    UnmapViewOfFile(m_base);
}

void *WebViewHostClientChannelWindows::WindowsImageBuffer::base() const {
    return m_base;
}

size_t WebViewHostClientChannelWindows::WindowsImageBuffer::size() const {
    return m_size;
}

int64_t WebViewHostClientChannelWindows::sendSharedImageBufferWithNextRequest(WebViewSharedImageBuffer *buffer) {
    if(buffer == nullptr)
        return 0;

    HANDLE outputHandle;

    if(!DuplicateHandle(
        GetCurrentProcess(), static_cast<WindowsImageBuffer *>(buffer)->handle(),
        m_process, &outputHandle,
        0,
        FALSE,
        DUPLICATE_SAME_ACCESS))
        throw std::runtime_error("DuplicateHandle has failed");

    return reinterpret_cast<int64_t>(outputHandle);
}

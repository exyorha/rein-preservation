#include "WebViewRPCServerWindows.h"

#include <include/wrapper/cef_closure_task.h>
#include <include/base/cef_callback.h>
#include <include/base/cef_logging.h>
#include <include/cef_app.h>


WebViewRPCServerWindows::WebViewRPCServerWindows(const std::wstring &server) : m_receiveBuffer(128 * 1024) {
    if(!WaitNamedPipe(server.c_str(), NMPWAIT_USE_DEFAULT_WAIT))
        throw std::runtime_error("WaitNamedPipe has failed");

    m_handle = CreateFile(server.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL, nullptr);
    if(!m_handle)
        throw std::runtime_error("CreateFile has failed");

    LOG(INFO) << "WebViewRPCServerWindows: connected to the server";

    m_requestReceivingThread = std::thread(&WebViewRPCServerWindows::receiveRequests, this);
}

WebViewRPCServerWindows::~WebViewRPCServerWindows() {
    CloseHandle(m_handle);
    clearCallInProgress();
    m_requestReceivingThread.join();
}

void WebViewRPCServerWindows::receiveRequests() {
    LOG(INFO) <<  "WebViewRPCServerWindows: receiving requests";

    runMainLoop();

    LOG(INFO) << "WebViewRPCServerWindows: main loop exited, signaling the browser UI thread to stop";

    CefPostTask(TID_UI, base::BindOnce(CefQuitMessageLoop));
}

void WebViewRPCServerWindows::runMainLoop() {
    while(true) {
        waitNoCallInProgress();

        DWORD bytesRead;

        if(!ReadFile(m_handle, m_receiveBuffer.data(), m_receiveBuffer.size(), &bytesRead, nullptr)) {
            LOG(ERROR) << "WebViewRPCServerWindows: ReadFile has failed";
            break;
        }

        LOG(INFO) << "WebViewRPCServerWindows: received " << bytesRead << " bytes";

        auto fullRequest = std::make_unique<webview::protocol::RPCRequest>();
        if(!fullRequest->ParseFromArray(m_receiveBuffer.data(), bytesRead)) {
            LOG(ERROR) << "WebViewRPCServer: failed to parse the RPC request";
            return;
        }

        setCallInProgress();

        CefPostTask(TID_UI, base::BindOnce(&WebViewRPCServerWindows::executeRPCCall, base::Unretained(this), std::move(fullRequest)));
    }
}


void WebViewRPCServerWindows::executeRPCCall(std::unique_ptr<webview::protocol::RPCRequest> &&request) {
    if(!executeRPCCallAndSendResponse(std::move(request))) {

        LOG(ERROR) << "WebViewRPCServerWindows: an error occurred during RPC processing";

        CefQuitMessageLoop();
    }
}

bool WebViewRPCServerWindows::executeRPCCallAndSendResponse(std::unique_ptr<webview::protocol::RPCRequest> &&request) {
    auto result = doExecuteRPCCall(std::move(request));
    if(!result.has_value()) {
        return false;
    }

    const auto &fullResponseData = *result;

    DWORD bytesWritten;
    if(!WriteFile(m_handle, fullResponseData.data(), fullResponseData.size(), &bytesWritten, nullptr) ||
        bytesWritten != fullResponseData.size()) {

        LOG(ERROR) << "WebViewRPCServerWindows: WriteFile has failed";
        return false;
    }

    clearCallInProgress();

    return true;
}

std::unique_ptr<WebViewSharedImageBuffer> WebViewRPCServerWindows::receiveImageBuffer(intptr_t handle) {
    if(handle == 0)
        return nullptr;

    return std::make_unique<WindowsSharedImageBuffer>(reinterpret_cast<HANDLE>(handle));
}

WebViewRPCServerWindows::WindowsSharedImageBuffer::WindowsSharedImageBuffer(HANDLE inHandle) {
    struct HandleHolder {
        inline HandleHolder(HANDLE handle) : handle(handle) {

        }

        ~HandleHolder() {
            CloseHandle(handle);
        }

        HANDLE handle;
    } handle(inHandle);

    m_base = MapViewOfFile(handle.handle, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
    if(!m_base)
        throw std::runtime_error("MapViewOfFile has failed");

    MEMORY_BASIC_INFORMATION info;
    if(VirtualQuery(m_base, &info, sizeof(info)) != sizeof(info)) {
        UnmapViewOfFile(m_base);
        throw std::runtime_error("VirtualQuery has failed");
    }

    m_size = info.RegionSize;
}

WebViewRPCServerWindows::WindowsSharedImageBuffer::~WindowsSharedImageBuffer() {
    UnmapViewOfFile(m_base);
}

void *WebViewRPCServerWindows::WindowsSharedImageBuffer::base() const {
    return m_base;
}

size_t WebViewRPCServerWindows::WindowsSharedImageBuffer::size() const {
    return m_size;
}

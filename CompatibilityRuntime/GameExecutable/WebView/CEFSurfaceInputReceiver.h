#ifndef WEBVIEW_CEF_SURFACE_INPUT_RECEIVER_H
#define WEBVIEW_CEF_SURFACE_INPUT_RECEIVER_H

namespace webview::protocol {
    class RPCMessage;
};

class CEFSurfaceInputReceiver {
protected:
    CEFSurfaceInputReceiver();
    ~CEFSurfaceInputReceiver();

public:
    CEFSurfaceInputReceiver(const CEFSurfaceInputReceiver &other) = delete;
    CEFSurfaceInputReceiver &operator =(const CEFSurfaceInputReceiver &other) = delete;

    virtual void forwardInputEvent(const webview::protocol::RPCMessage &message) = 0;
};

#endif


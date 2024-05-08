#ifndef WEB_VIEW_HOST_WEB_VIEW_RPC_SERVICE_H
#define WEB_VIEW_HOST_WEB_VIEW_RPC_SERVICE_H

#include <WebView/WebViewProtocol.pb.h>

#include <vector>
#include <memory>

#include "WebViewRPCImplementation.h"

class WebView;

class WebViewRPCService final : public webview::protocol::WebViewService {
public:
    WebViewRPCService();
    ~WebViewRPCService() override;

    void AddJavaScript(::google::protobuf::RpcController *controller, const ::webview::protocol::AddJavaScriptRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void AddPermissionTrustDomain(::google::protobuf::RpcController *controller, const ::webview::protocol::AddPermissionTrustDomainRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void AddSslExceptionDomain(::google::protobuf::RpcController *controller, const ::webview::protocol::AddSslExceptionDomainRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void AddUrlScheme(::google::protobuf::RpcController *controller, const ::webview::protocol::AddUrlSchemeRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void AnimateTo(::google::protobuf::RpcController *controller, const ::webview::protocol::AnimateToRequest *request, ::webview::protocol::BoolResponse *response, google::protobuf::Closure *done) override;
    void CanGoBack(::google::protobuf::RpcController *controller, const ::webview::protocol::CanGoBackRequest *request, ::webview::protocol::BoolResponse *response, google::protobuf::Closure *done) override;
    void CanGoForward(::google::protobuf::RpcController *controller, const ::webview::protocol::CanGoForwardRequest *request, ::webview::protocol::BoolResponse *response, google::protobuf::Closure *done) override;
    void CleanCache(::google::protobuf::RpcController *controller, const ::webview::protocol::CleanCacheRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void ClearCookies(::google::protobuf::RpcController *controller, const ::webview::protocol::ClearCookiesRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void ClearHttpAuthUsernamePassword(::google::protobuf::RpcController *controller, const ::webview::protocol::ClearHttpAuthUsernamePasswordRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void Destroy(::google::protobuf::RpcController *controller, const ::webview::protocol::DestroyRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void EvaluateJavaScript(::google::protobuf::RpcController *controller, const ::webview::protocol::EvaluateJavaScriptRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void GetCookie(::google::protobuf::RpcController *controller, const ::webview::protocol::GetCookieRequest *request, ::webview::protocol::StringResponse *response, google::protobuf::Closure *done) override;
    void GetUrl(::google::protobuf::RpcController *controller, const ::webview::protocol::GetUrlRequest *request, ::webview::protocol::StringResponse *response, google::protobuf::Closure *done) override;
    void GetUserAgent(::google::protobuf::RpcController *controller, const ::webview::protocol::GetUserAgentRequest *request, ::webview::protocol::StringResponse *response, google::protobuf::Closure *done) override;
    void GetWebViewAlpha(::google::protobuf::RpcController *controller, const ::webview::protocol::GetWebViewAlphaRequest *request, ::webview::protocol::FloatResponse *response, google::protobuf::Closure *done) override;
    void GoBack(::google::protobuf::RpcController *controller, const ::webview::protocol::GoBackRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void GoForward(::google::protobuf::RpcController *controller, const ::webview::protocol::GoForwardRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void Hide(::google::protobuf::RpcController *controller, const ::webview::protocol::HideRequest *request, ::webview::protocol::BoolResponse *response, google::protobuf::Closure *done) override;
    void Init(::google::protobuf::RpcController *controller, const ::webview::protocol::InitRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void Load(::google::protobuf::RpcController *controller, const ::webview::protocol::LoadRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void LoadHTMLString(::google::protobuf::RpcController *controller, const ::webview::protocol::LoadHTMLStringRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void Print(::google::protobuf::RpcController *controller, const ::webview::protocol::PrintRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void Reload(::google::protobuf::RpcController *controller, const ::webview::protocol::ReloadRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void RemovePermissionTrustDomain(::google::protobuf::RpcController *controller, const ::webview::protocol::RemovePermissionTrustDomainRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void RemoveSslExceptionDomain(::google::protobuf::RpcController *controller, const ::webview::protocol::RemoveSslExceptionDomainRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void RemoveUrlScheme(::google::protobuf::RpcController *controller, const ::webview::protocol::RemoveUrlSchemeRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void ScrollTo(::google::protobuf::RpcController *controller, const ::webview::protocol::ScrollToRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void SetAllowAutoPlay(::google::protobuf::RpcController *controller, const ::webview::protocol::SetAllowAutoPlayRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void SetAllowHTTPAuthPopUpWindow(::google::protobuf::RpcController *controller, const ::webview::protocol::SetAllowHTTPAuthPopUpWindowRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void SetAllowJavaScriptOpenWindow(::google::protobuf::RpcController *controller, const ::webview::protocol::SetAllowJavaScriptOpenWindowRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void SetBackButtonEnabled(::google::protobuf::RpcController *controller, const ::webview::protocol::SetBackButtonEnabledRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void SetBackgroundColor(::google::protobuf::RpcController *controller, const ::webview::protocol::SetBackgroundColorRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void SetBouncesEnabled(::google::protobuf::RpcController *controller, const ::webview::protocol::SetBouncesEnabledRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void SetCalloutEnabled(::google::protobuf::RpcController *controller, const ::webview::protocol::SetCalloutEnabledRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void SetCookie(::google::protobuf::RpcController *controller, const ::webview::protocol::SetCookieRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void SetDefaultFontSize(::google::protobuf::RpcController *controller, const ::webview::protocol::SetDefaultFontSizeRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void SetFrame(::google::protobuf::RpcController *controller, const ::webview::protocol::SetFrameRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void SetHeaderField(::google::protobuf::RpcController *controller, const ::webview::protocol::SetHeaderFieldRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void SetHorizontalScrollBarEnabled(::google::protobuf::RpcController *controller, const ::webview::protocol::SetHorizontalScrollBarEnabledRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void SetImmersiveModeEnabled(::google::protobuf::RpcController *controller, const ::webview::protocol::SetImmersiveModeEnabledRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void SetJavaScriptEnabled(::google::protobuf::RpcController *controller, const ::webview::protocol::SetJavaScriptEnabledRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void SetLoadWithOverviewMode(::google::protobuf::RpcController *controller, const ::webview::protocol::SetLoadWithOverviewModeRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void SetLogLevel(::google::protobuf::RpcController *controller, const ::webview::protocol::SetLogLevelRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void SetOpenLinksInExternalBrowser(::google::protobuf::RpcController *controller, const ::webview::protocol::SetOpenLinksInExternalBrowserRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void SetPosition(::google::protobuf::RpcController *controller, const ::webview::protocol::SetPositionRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void SetShowSpinnerWhileLoading(::google::protobuf::RpcController *controller, const ::webview::protocol::SetShowSpinnerWhileLoadingRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void SetSize(::google::protobuf::RpcController *controller, const ::webview::protocol::SetSizeRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void SetSpinnerText(::google::protobuf::RpcController *controller, const ::webview::protocol::SetSpinnerTextRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void SetSupportMultipleWindows(::google::protobuf::RpcController *controller, const ::webview::protocol::SetSupportMultipleWindowsRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void SetUseWideViewPort(::google::protobuf::RpcController *controller, const ::webview::protocol::SetUseWideViewPortRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void SetUserAgent(::google::protobuf::RpcController *controller, const ::webview::protocol::SetUserAgentRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void SetUserInteractionEnabled(::google::protobuf::RpcController *controller, const ::webview::protocol::SetUserInteractionEnabledRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void SetVerticalScrollBarEnabled(::google::protobuf::RpcController *controller, const ::webview::protocol::SetVerticalScrollBarEnabledRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void SetWebContentsDebuggingEnabled(::google::protobuf::RpcController *controller, const ::webview::protocol::SetWebContentsDebuggingEnabledRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void SetWebViewAlpha(::google::protobuf::RpcController *controller, const ::webview::protocol::SetWebViewAlphaRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void SetZoomEnabled(::google::protobuf::RpcController *controller, const ::webview::protocol::SetZoomEnabledRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void Show(::google::protobuf::RpcController *controller, const ::webview::protocol::ShowRequest *request, ::webview::protocol::BoolResponse *response, google::protobuf::Closure *done) override;
    void ShowWebViewDialog(::google::protobuf::RpcController *controller, const ::webview::protocol::ShowWebViewDialogRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;
    void Stop(::google::protobuf::RpcController *controller, const ::webview::protocol::StopRequest *request, ::webview::protocol::VoidResponse *response, google::protobuf::Closure *done) override;

    inline void handleNonCallMessage(std::unique_ptr<webview::protocol::RPCMessage> &&request) {
        m_impl.handleNonCallMessage(std::move(request));
    }

private:
    WebViewRPCImplementation m_impl;

};

#endif

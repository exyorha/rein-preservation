#include "WebViewRPCService.h"
#include "WebView.h"

WebViewRPCService::WebViewRPCService() = default;

WebViewRPCService::~WebViewRPCService() = default;

void WebViewRPCService::AddJavaScript(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::AddJavaScriptRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.addJavaScript(request->arg1(), request->arg2(), request->arg3());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::AddPermissionTrustDomain(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::AddPermissionTrustDomainRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.addPermissionTrustDomain(request->arg1(), request->arg2());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::AddSslExceptionDomain(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::AddSslExceptionDomainRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.addSslExceptionDomain(request->arg1(), request->arg2());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::AddUrlScheme(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::AddUrlSchemeRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.addUrlScheme(request->arg1(), request->arg2());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::AnimateTo(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::AnimateToRequest *request,
  ::webview::protocol::BoolResponse *response,
  google::protobuf::Closure *done)
{

  try {
    bool result = m_impl.animateTo(request->arg1(), request->arg2(), request->arg3(), request->arg4(), request->arg5(), request->arg6(), request->arg7(), request->arg8());
    response->set_result(result);
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::CanGoBack(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::CanGoBackRequest *request,
  ::webview::protocol::BoolResponse *response,
  google::protobuf::Closure *done)
{

  try {
    bool result = m_impl.canGoBack(request->arg1());
    response->set_result(result);
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::CanGoForward(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::CanGoForwardRequest *request,
  ::webview::protocol::BoolResponse *response,
  google::protobuf::Closure *done)
{

  try {
    bool result = m_impl.canGoForward(request->arg1());
    response->set_result(result);
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::CleanCache(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::CleanCacheRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.cleanCache(request->arg1());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::ClearCookies(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::ClearCookiesRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.clearCookies();
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::ClearHttpAuthUsernamePassword(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::ClearHttpAuthUsernamePasswordRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.clearHttpAuthUsernamePassword(request->arg1(), request->arg2());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::Destroy(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::DestroyRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.destroy(request->arg1());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::EvaluateJavaScript(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::EvaluateJavaScriptRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.evaluateJavaScript(request->arg1(), request->arg2(), request->arg3());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::GetCookie(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::GetCookieRequest *request,
  ::webview::protocol::StringResponse *response,
  google::protobuf::Closure *done)
{

  try {
    std::string result = m_impl.getCookie(request->arg1(), request->arg2());
    response->set_result(result);
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::GetUrl(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::GetUrlRequest *request,
  ::webview::protocol::StringResponse *response,
  google::protobuf::Closure *done)
{

  try {
    std::string result = m_impl.getUrl(request->arg1());
    response->set_result(result);
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::GetUserAgent(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::GetUserAgentRequest *request,
  ::webview::protocol::StringResponse *response,
  google::protobuf::Closure *done)
{

  try {
    std::string result = m_impl.getUserAgent(request->arg1());
    response->set_result(result);
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::GetWebViewAlpha(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::GetWebViewAlphaRequest *request,
  ::webview::protocol::FloatResponse *response,
  google::protobuf::Closure *done)
{

  try {
    float result = m_impl.getWebViewAlpha(request->arg1());
    response->set_result(result);
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::GoBack(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::GoBackRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.goBack(request->arg1());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::GoForward(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::GoForwardRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.goForward(request->arg1());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::Hide(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::HideRequest *request,
  ::webview::protocol::BoolResponse *response,
  google::protobuf::Closure *done)
{

  try {
    bool result = m_impl.hide(request->arg1(), request->arg2(), request->arg3(), request->arg4(), request->arg5());
    response->set_result(result);
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::Init(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::InitRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.init(request->arg1(), request->arg2(), request->arg3(), request->arg4(), request->arg5(), request->parentwindowhandle());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::Load(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::LoadRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.load(request->arg1(), request->arg2());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::LoadHTMLString(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::LoadHTMLStringRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.loadHTMLString(request->arg1(), request->arg2(), request->arg3());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::Print(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::PrintRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.print(request->arg1());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::Reload(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::ReloadRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.reload(request->arg1());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::RemovePermissionTrustDomain(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::RemovePermissionTrustDomainRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.removePermissionTrustDomain(request->arg1(), request->arg2());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::RemoveSslExceptionDomain(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::RemoveSslExceptionDomainRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.removeSslExceptionDomain(request->arg1(), request->arg2());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::RemoveUrlScheme(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::RemoveUrlSchemeRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.removeUrlScheme(request->arg1(), request->arg2());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}
\
void WebViewRPCService::ScrollTo(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::ScrollToRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.scrollTo(request->arg1(), request->arg2(), request->arg3(), request->arg4());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::SetAllowAutoPlay(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::SetAllowAutoPlayRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.setAllowAutoPlay(request->arg1());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::SetAllowHTTPAuthPopUpWindow(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::SetAllowHTTPAuthPopUpWindowRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.setAllowHTTPAuthPopUpWindow(request->arg1(), request->arg2());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::SetAllowJavaScriptOpenWindow(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::SetAllowJavaScriptOpenWindowRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.setAllowJavaScriptOpenWindow(request->arg1());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::SetBackButtonEnabled(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::SetBackButtonEnabledRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.setBackButtonEnabled(request->arg1(), request->arg2());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::SetBackgroundColor(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::SetBackgroundColorRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.setBackgroundColor(request->arg1(), request->arg2(), request->arg3(), request->arg4(), request->arg5());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::SetBouncesEnabled(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::SetBouncesEnabledRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.setBouncesEnabled(request->arg1(), request->arg2());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::SetCalloutEnabled(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::SetCalloutEnabledRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.setCalloutEnabled(request->arg1(), request->arg2());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::SetCookie(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::SetCookieRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.setCookie(request->arg1(), request->arg2());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::SetDefaultFontSize(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::SetDefaultFontSizeRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.setDefaultFontSize(request->arg1(), request->arg2());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::SetFrame(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::SetFrameRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.setFrame(request->arg1(), request->arg2(), request->arg3(), request->arg4(), request->arg5());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::SetHeaderField(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::SetHeaderFieldRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.setHeaderField(request->arg1(), request->arg2(), request->arg3());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::SetHorizontalScrollBarEnabled(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::SetHorizontalScrollBarEnabledRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.setHorizontalScrollBarEnabled(request->arg1(), request->arg2());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::SetImmersiveModeEnabled(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::SetImmersiveModeEnabledRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.setImmersiveModeEnabled(request->arg1(), request->arg2());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::SetJavaScriptEnabled(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::SetJavaScriptEnabledRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.setJavaScriptEnabled(request->arg1());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::SetLoadWithOverviewMode(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::SetLoadWithOverviewModeRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.setLoadWithOverviewMode(request->arg1(), request->arg2());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::SetLogLevel(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::SetLogLevelRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.setLogLevel(request->arg1());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::SetOpenLinksInExternalBrowser(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::SetOpenLinksInExternalBrowserRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.setOpenLinksInExternalBrowser(request->arg1(), request->arg2());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::SetPosition(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::SetPositionRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.setPosition(request->arg1(), request->arg2(), request->arg3());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::SetShowSpinnerWhileLoading(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::SetShowSpinnerWhileLoadingRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.setShowSpinnerWhileLoading(request->arg1(), request->arg2());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::SetSize(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::SetSizeRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.setSize(request->arg1(), request->arg2(), request->arg3());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::SetSpinnerText(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::SetSpinnerTextRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.setSpinnerText(request->arg1(), request->arg2());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::SetSupportMultipleWindows(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::SetSupportMultipleWindowsRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.setSupportMultipleWindows(request->arg1(), request->arg2());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::SetUseWideViewPort(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::SetUseWideViewPortRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.setUseWideViewPort(request->arg1(), request->arg2());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::SetUserAgent(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::SetUserAgentRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.setUserAgent(request->arg1(), request->arg2());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::SetUserInteractionEnabled(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::SetUserInteractionEnabledRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.setUserInteractionEnabled(request->arg1(), request->arg2());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::SetVerticalScrollBarEnabled(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::SetVerticalScrollBarEnabledRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.setVerticalScrollBarEnabled(request->arg1(), request->arg2());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::SetWebContentsDebuggingEnabled(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::SetWebContentsDebuggingEnabledRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.setWebContentsDebuggingEnabled(request->arg1());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::SetWebViewAlpha(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::SetWebViewAlphaRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.setWebViewAlpha(request->arg1(), request->arg2());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::SetZoomEnabled(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::SetZoomEnabledRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.setZoomEnabled(request->arg1(), request->arg2());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::Show(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::ShowRequest *request,
  ::webview::protocol::BoolResponse *response,
  google::protobuf::Closure *done)
{

  try {
    bool result = m_impl.show(request->arg1(), request->arg2(), request->arg3(), request->arg4(), request->arg5());
    response->set_result(result);
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::ShowWebViewDialog(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::ShowWebViewDialogRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.showWebViewDialog(request->arg1(), request->arg2());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}

void WebViewRPCService::Stop(
  ::google::protobuf::RpcController *controller,
  const ::webview::protocol::StopRequest *request,
  ::webview::protocol::VoidResponse *response,
  google::protobuf::Closure *done)
{

  try {
    m_impl.stop(request->arg1());
  } catch(const std::exception &e) {
    controller->SetFailed(e.what());
  } catch(...) {
    controller->SetFailed("no message available");
  }
}


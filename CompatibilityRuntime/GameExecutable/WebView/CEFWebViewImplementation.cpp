#include <WebView/CEFWebViewImplementation.h>
#include <WebView/CEFSurface.h>

#include <translator_api.h>

CEFWebViewImplementation::CEFWebViewImplementation(const WebViewHostClientConfiguration &config) : m_client(config),
    m_installer(this) {

    /*
     * TODO if needed:
        auto dpi = reinterpret_cast<float (*)()>(translator_resolve_native_icall("UnityEngine.Screen::get_dpi"))();
     */
}

CEFWebViewImplementation::~CEFWebViewImplementation() = default;

float CEFWebViewImplementation::screenHeight() {
    return screenHeightStatic();
}

float CEFWebViewImplementation::screenWidth() {
    return screenWidthStatic();
}

int32_t CEFWebViewImplementation::screenHeightStatic() {
    using ScreenHeightPtr = uint32_t (*)();

    static ScreenHeightPtr screenHeightPtr = nullptr;
    if(!screenHeightPtr) {
        screenHeightPtr = reinterpret_cast<ScreenHeightPtr>(translator_resolve_native_icall("UnityEngine.Screen::get_height"));
    }

    return screenHeightPtr();
}

int32_t CEFWebViewImplementation::screenWidthStatic() {
    using ScreenWidthPtr = uint32_t (*)();

    static ScreenWidthPtr screenWidthPtr = nullptr;
    if(!screenWidthPtr) {
        screenWidthPtr = reinterpret_cast<ScreenWidthPtr>(translator_resolve_native_icall("UnityEngine.Screen::get_width"));
    }

    return screenWidthPtr();
}

void CEFWebViewImplementation::addJavaScript(const std::string & arg1, const std::string & arg2, const std::string & arg3) {
    webview::protocol::AddJavaScriptRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    request.set_arg3(arg3);
    m_client.stub().AddJavaScript(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::addPermissionTrustDomain(const std::string & arg1, const std::string & arg2) {
    webview::protocol::AddPermissionTrustDomainRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    m_client.stub().AddPermissionTrustDomain(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::addSslExceptionDomain(const std::string & arg1, const std::string & arg2) {
    webview::protocol::AddSslExceptionDomainRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    m_client.stub().AddSslExceptionDomain(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::addUrlScheme(const std::string & arg1, const std::string & arg2) {
    webview::protocol::AddUrlSchemeRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    m_client.stub().AddUrlScheme(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

bool CEFWebViewImplementation::animateTo(const std::string & arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5, float arg6, float arg7, const std::string & arg8) {
    webview::protocol::AnimateToRequest request;
    webview::protocol::BoolResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    request.set_arg3(arg3);
    request.set_arg4(arg4);
    request.set_arg5(arg5);
    request.set_arg6(arg6);
    request.set_arg7(arg7);
    request.set_arg8(arg8);
    m_client.stub().AnimateTo(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
    return response.result();
}

bool CEFWebViewImplementation::canGoBack(const std::string & arg1) {
    webview::protocol::CanGoBackRequest request;
    webview::protocol::BoolResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    m_client.stub().CanGoBack(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
    return response.result();
}

bool CEFWebViewImplementation::canGoForward(const std::string & arg1) {
    webview::protocol::CanGoForwardRequest request;
    webview::protocol::BoolResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    m_client.stub().CanGoForward(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
    return response.result();
}

void CEFWebViewImplementation::cleanCache(const std::string & arg1) {
    webview::protocol::CleanCacheRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    m_client.stub().CleanCache(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::clearCookies() {
    webview::protocol::ClearCookiesRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    m_client.stub().ClearCookies(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::clearHttpAuthUsernamePassword(const std::string & arg1, const std::string & arg2) {
    webview::protocol::ClearHttpAuthUsernamePasswordRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    m_client.stub().ClearHttpAuthUsernamePassword(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::destroy(const std::string & arg1) {
    {
        std::unique_lock<std::mutex> locker(m_surfacesMutex);
        m_surfaces.erase(arg1);
    }

    webview::protocol::DestroyRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    m_client.stub().Destroy(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::evaluateJavaScript(const std::string & arg1, const std::string & arg2, const std::string & arg3) {
    webview::protocol::EvaluateJavaScriptRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    request.set_arg3(arg3);
    m_client.stub().EvaluateJavaScript(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

std::string CEFWebViewImplementation::getCookie(const std::string & arg1, const std::string & arg2) {
    webview::protocol::GetCookieRequest request;
    webview::protocol::StringResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    m_client.stub().GetCookie(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
    return response.result();
}

std::string CEFWebViewImplementation::getUrl(const std::string & arg1) {
    webview::protocol::GetUrlRequest request;
    webview::protocol::StringResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    m_client.stub().GetUrl(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
    return response.result();
}

std::string CEFWebViewImplementation::getUserAgent(const std::string & arg1) {
    webview::protocol::GetUserAgentRequest request;
    webview::protocol::StringResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    m_client.stub().GetUserAgent(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
    return response.result();
}

float CEFWebViewImplementation::getWebViewAlpha(const std::string & arg1) {
    webview::protocol::GetWebViewAlphaRequest request;
    webview::protocol::FloatResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    m_client.stub().GetWebViewAlpha(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
    return response.result();
}

void CEFWebViewImplementation::goBack(const std::string & arg1) {
    webview::protocol::GoBackRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    m_client.stub().GoBack(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::goForward(const std::string & arg1) {
    webview::protocol::GoForwardRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    m_client.stub().GoForward(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

bool CEFWebViewImplementation::hide(const std::string & arg1, bool arg2, int32_t arg3, float arg4, const std::string & arg5) {
    {
        std::unique_lock<std::mutex> locker(m_surfacesMutex);
        auto surface = getSurfaceLocked(arg1);
        surface->setHidden(true);
    }

    webview::protocol::HideRequest request;
    webview::protocol::BoolResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    request.set_arg3(arg3);
    request.set_arg4(arg4);
    request.set_arg5(arg5);
    m_client.stub().Hide(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
    return response.result();
}

void CEFWebViewImplementation::init(const std::string & arg1, int32_t x, int32_t y, int32_t width, int32_t height) {
    std::unique_lock<std::mutex> locker(m_surfacesMutex);

    auto result = m_surfaces.emplace(arg1, nullptr);
    if(!result.second)
        throw std::runtime_error("a surface with the specified ID already exists");

    width = std::max<int32_t>(width, 1);
    height = std::max<int32_t>(height, 1);

    auto buffer = m_client.channel().allocateImageBuffer(WebViewSharedImageBuffer::sizeBuffer(width, height));

    webview::protocol::InitRequest request;
    webview::protocol::VoidResponse response;

    request.set_sharedmemoryregion(m_client.channel().sendSharedImageBufferWithNextRequest(buffer.get()));

    auto surface = std::make_unique<CEFSurface>(x, y, width, height, std::move(buffer));

    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(x);
    request.set_arg3(y);
    request.set_arg4(width);
    request.set_arg5(height);
    m_client.stub().Init(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        m_surfaces.erase(result.first);

        throw std::runtime_error(ctrl.ErrorText());
    }

    result.first->second = std::move(surface);
}

void CEFWebViewImplementation::load(const std::string & arg1, const std::string & arg2) {
    printf("CEFWebViewImplementation::load: %s\n", arg2.c_str());

    webview::protocol::LoadRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    m_client.stub().Load(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::loadHTMLString(const std::string & arg1, const std::string & arg2, const std::string & arg3) {
    webview::protocol::LoadHTMLStringRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    request.set_arg3(arg3);
    m_client.stub().LoadHTMLString(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::print(const std::string & arg1) {
    webview::protocol::PrintRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    m_client.stub().Print(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::reload(const std::string & arg1) {
    webview::protocol::ReloadRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    m_client.stub().Reload(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::removePermissionTrustDomain(const std::string & arg1, const std::string & arg2) {
    webview::protocol::RemovePermissionTrustDomainRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    m_client.stub().RemovePermissionTrustDomain(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::removeSslExceptionDomain(const std::string & arg1, const std::string & arg2) {
    webview::protocol::RemoveSslExceptionDomainRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    m_client.stub().RemoveSslExceptionDomain(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::removeUrlScheme(const std::string & arg1, const std::string & arg2) {
    webview::protocol::RemoveUrlSchemeRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    m_client.stub().RemoveUrlScheme(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::scrollTo(const std::string & arg1, int32_t arg2, int32_t arg3, bool arg4) {
    webview::protocol::ScrollToRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    request.set_arg3(arg3);
    request.set_arg4(arg4);
    m_client.stub().ScrollTo(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::setAllowAutoPlay(bool arg1) {
    webview::protocol::SetAllowAutoPlayRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    m_client.stub().SetAllowAutoPlay(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::setAllowHTTPAuthPopUpWindow(const std::string & arg1, bool arg2) {
    webview::protocol::SetAllowHTTPAuthPopUpWindowRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    m_client.stub().SetAllowHTTPAuthPopUpWindow(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::setAllowJavaScriptOpenWindow(bool arg1) {
    webview::protocol::SetAllowJavaScriptOpenWindowRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    m_client.stub().SetAllowJavaScriptOpenWindow(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::setBackButtonEnabled(const std::string & arg1, bool arg2) {
    webview::protocol::SetBackButtonEnabledRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    m_client.stub().SetBackButtonEnabled(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::setBackgroundColor(const std::string & arg1, float arg2, float arg3, float arg4, float arg5) {
    webview::protocol::SetBackgroundColorRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    request.set_arg3(arg3);
    request.set_arg4(arg4);
    request.set_arg5(arg5);
    m_client.stub().SetBackgroundColor(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::setBouncesEnabled(const std::string & arg1, bool arg2) {
    webview::protocol::SetBouncesEnabledRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    m_client.stub().SetBouncesEnabled(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::setCalloutEnabled(const std::string & arg1, bool arg2) {
    webview::protocol::SetCalloutEnabledRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    m_client.stub().SetCalloutEnabled(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::setCookie(const std::string & arg1, const std::string & arg2) {
    webview::protocol::SetCookieRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    m_client.stub().SetCookie(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::setDefaultFontSize(const std::string & arg1, int32_t arg2) {
    webview::protocol::SetDefaultFontSizeRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    m_client.stub().SetDefaultFontSize(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::setFrame(const std::string & arg1, int32_t x, int32_t y, int32_t width, int32_t height) {
    width = std::max<int32_t>(width, 1);
    height = std::max<int32_t>(height, 1);

    webview::protocol::SetFrameRequest request;
    webview::protocol::VoidResponse response;

    {
        std::unique_lock<std::mutex> locker(m_surfacesMutex);
        auto surface = getSurfaceLocked(arg1);

        auto buffer = m_client.channel().allocateImageBuffer(WebViewSharedImageBuffer::sizeBuffer(width, height));
        request.set_sharedmemoryregion(m_client.channel().sendSharedImageBufferWithNextRequest(buffer.get()));

        surface->setFrame(x, y, width, height, std::move(buffer));
    }

    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(x);
    request.set_arg3(y);
    request.set_arg4(width);
    request.set_arg5(height);
    m_client.stub().SetFrame(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::setHeaderField(const std::string & arg1, const std::string & arg2, const std::string & arg3) {
    webview::protocol::SetHeaderFieldRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    request.set_arg3(arg3);
    m_client.stub().SetHeaderField(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::setHorizontalScrollBarEnabled(const std::string & arg1, bool arg2) {
    webview::protocol::SetHorizontalScrollBarEnabledRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    m_client.stub().SetHorizontalScrollBarEnabled(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::setImmersiveModeEnabled(const std::string & arg1, bool arg2) {
    webview::protocol::SetImmersiveModeEnabledRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    m_client.stub().SetImmersiveModeEnabled(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::setJavaScriptEnabled(bool arg1) {
    webview::protocol::SetJavaScriptEnabledRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    m_client.stub().SetJavaScriptEnabled(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::setLoadWithOverviewMode(const std::string & arg1, bool arg2) {
    webview::protocol::SetLoadWithOverviewModeRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    m_client.stub().SetLoadWithOverviewMode(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::setLogLevel(int32_t arg1) {
    webview::protocol::SetLogLevelRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    m_client.stub().SetLogLevel(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::setOpenLinksInExternalBrowser(const std::string & arg1, bool arg2) {
    webview::protocol::SetOpenLinksInExternalBrowserRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    m_client.stub().SetOpenLinksInExternalBrowser(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::setPosition(const std::string & arg1, int32_t x, int32_t y) {

    {
        std::unique_lock<std::mutex> locker(m_surfacesMutex);
        auto surface = getSurfaceLocked(arg1);
        surface->setPosition(x, y);
    }

    webview::protocol::SetPositionRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(x);
    request.set_arg3(y);
    m_client.stub().SetPosition(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::setShowSpinnerWhileLoading(const std::string & arg1, bool arg2) {
    webview::protocol::SetShowSpinnerWhileLoadingRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    m_client.stub().SetShowSpinnerWhileLoading(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::setSize(const std::string & arg1, int32_t width, int32_t height) {
    width = std::max<int32_t>(width, 1);
    height = std::max<int32_t>(height, 1);

    webview::protocol::SetSizeRequest request;
    webview::protocol::VoidResponse response;

    {
        std::unique_lock<std::mutex> locker(m_surfacesMutex);
        auto surface = getSurfaceLocked(arg1);

        auto buffer = m_client.channel().allocateImageBuffer(WebViewSharedImageBuffer::sizeBuffer(width, height));
        request.set_sharedmemoryregion(m_client.channel().sendSharedImageBufferWithNextRequest(buffer.get()));

        surface->setSize(width, height, std::move(buffer));
    }


    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(width);
    request.set_arg3(height);
    m_client.stub().SetSize(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::setSpinnerText(const std::string & arg1, const std::string & arg2) {
    webview::protocol::SetSpinnerTextRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    m_client.stub().SetSpinnerText(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::setSupportMultipleWindows(const std::string & arg1, bool arg2) {
    webview::protocol::SetSupportMultipleWindowsRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    m_client.stub().SetSupportMultipleWindows(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::setUseWideViewPort(const std::string & arg1, bool arg2) {
    webview::protocol::SetUseWideViewPortRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    m_client.stub().SetUseWideViewPort(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::setUserAgent(const std::string & arg1, const std::string & arg2) {
    webview::protocol::SetUserAgentRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    m_client.stub().SetUserAgent(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::setUserInteractionEnabled(const std::string & arg1, bool arg2) {
    webview::protocol::SetUserInteractionEnabledRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    m_client.stub().SetUserInteractionEnabled(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::setVerticalScrollBarEnabled(const std::string & arg1, bool arg2) {
    webview::protocol::SetVerticalScrollBarEnabledRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    m_client.stub().SetVerticalScrollBarEnabled(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::setWebContentsDebuggingEnabled(bool arg1) {
    webview::protocol::SetWebContentsDebuggingEnabledRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    m_client.stub().SetWebContentsDebuggingEnabled(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::setWebViewAlpha(const std::string & arg1, float arg2) {
    webview::protocol::SetWebViewAlphaRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    m_client.stub().SetWebViewAlpha(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::setZoomEnabled(const std::string & arg1, bool arg2) {
    webview::protocol::SetZoomEnabledRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    m_client.stub().SetZoomEnabled(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

bool CEFWebViewImplementation::show(const std::string & arg1, bool arg2, int32_t arg3, float arg4, const std::string & arg5) {
    {
        std::unique_lock<std::mutex> locker(m_surfacesMutex);
        auto surface = getSurfaceLocked(arg1);

        surface->setHidden(false);
    }

    webview::protocol::ShowRequest request;
    webview::protocol::BoolResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    request.set_arg3(arg3);
    request.set_arg4(arg4);
    request.set_arg5(arg5);
    m_client.stub().Show(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
    return response.result();
}

void CEFWebViewImplementation::showWebViewDialog(const std::string & arg1, bool arg2) {
    webview::protocol::ShowWebViewDialogRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    request.set_arg2(arg2);
    m_client.stub().ShowWebViewDialog(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

void CEFWebViewImplementation::stop(const std::string & arg1) {
    webview::protocol::StopRequest request;
    webview::protocol::VoidResponse response;
    WebViewProtocolController ctrl;
    request.set_arg1(arg1);
    m_client.stub().Stop(&ctrl, &request, &response, nullptr);

    if(ctrl.Failed()) {
        throw std::runtime_error(ctrl.ErrorText());
    }
}

CEFSurface *CEFWebViewImplementation::getSurfaceLocked(const std::string &name) const {
    auto it = m_surfaces.find(name);
    if(it == m_surfaces.end() || !it->second)
        throw std::runtime_error("no such surface");

    return it->second.get();
}

void CEFWebViewImplementation::beforeSwapBuffers(int32_t width, int32_t height) {
    if(!m_compositor.has_value())
        return;

    m_compositor->disposeStaleTextures();

    bool renderedAny = false;

    {
        std::unique_lock<std::mutex> locker(m_surfacesMutex);

        for(const auto &surfacePair: m_surfaces) {
            auto surface = surfacePair.second.get();
            if(!surface)
                continue;

            if(surface->shouldRender()) {
                if(!renderedAny) {
                    renderedAny = true;
                    m_compositor->beforeRender();
                }

                m_compositor->renderSurface(surface, width, height);
            }
        }
    }

    if(renderedAny) {
        m_compositor->afterRender();
    }
}

void CEFWebViewImplementation::afterSwapBuffers() {
    std::unique_lock<std::mutex> locker(m_surfacesMutex);
    for(const auto &surfacePair: m_surfaces) {
        auto surface = surfacePair.second.get();
        if(!surface)
            continue;

        if(surface->getAndClearNeedsUploadFlag()) {
            if(!m_compositor.has_value()) {
                m_compositor.emplace();
            }

            m_compositor->uploadSurface(surface);
        }
    }
}

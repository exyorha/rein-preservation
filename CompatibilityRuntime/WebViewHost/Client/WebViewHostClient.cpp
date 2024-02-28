#include <WebViewHostClient.h>
#include <WebViewHostClientConfiguration.h>

#include <vector>
#include <string>
#include <optional>

WebViewHostClient::WebViewHostClient(const WebViewHostClientConfiguration &config) : m_channel(config) {

    m_stub.emplace(&m_channel);
}

WebViewHostClient::~WebViewHostClient() {

}

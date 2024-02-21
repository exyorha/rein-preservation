#include <include/cef_command_line.h>
#include "WebViewApp.h"
#include "ParentedContainerWindow.h"
#include "WebViewRPCServer.h"

int main(int argc, char **argv) {
    CefMainArgs mainArgs(argc, argv);

    CefRefPtr<WebViewApp> app(new WebViewApp);

    int exitCode = CefExecuteProcess(mainArgs, app.get(), nullptr);
    if (exitCode >= 0) {
        // The sub-process terminated, exit now.
        return exitCode;
    }

    CefSettings settings;
    if(!CefInitialize(mainArgs, settings, app.get(), nullptr))
        return 0;

    struct CefShutdownScope {
        inline ~CefShutdownScope() {
            CefShutdown();
        }
    } shutdownScope;

    auto commandLine = CefCommandLine::GetGlobalCommandLine();
#if 0
    if(!commandLine->HasSwitch("webview-parent-window")) {
        fprintf(stderr, "--webview-parent-window was not specified\n");
        return 1;
    }

    ParentedContainerWindow container(strtoull(commandLine->GetSwitchValue("webview-parent-window").ToString().c_str(), nullptr, 10));
#endif

    if(!commandLine->HasSwitch("webview-rpc-client-socket")) {
        fprintf(stderr, "--webview-rpc-client-socket was not specified\n");
        return 1;
    }

    WebViewRPCServer rpcServer(strtoll(commandLine->GetSwitchValue("webview-rpc-client-socket").ToString().c_str(), nullptr, 10));

    CefRunMessageLoop();

    return 0;
}

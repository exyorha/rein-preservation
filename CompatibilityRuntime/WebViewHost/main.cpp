#include <include/cef_command_line.h>

#include "WebViewApp.h"

#ifdef _WIN32
#include "WebViewRPCServerWindows.h"
#else
#include "WebViewRPCServerLinux.h"
#endif

static int applicationMain(const CefMainArgs &mainArgs) {

    CefRefPtr<WebViewApp> app(new WebViewApp);

    int exitCode = CefExecuteProcess(mainArgs, app.get(), nullptr);
    if (exitCode >= 0) {
        // The sub-process terminated, exit now.
        return exitCode;
    }

    auto earlyCommandLine = CefCommandLine::CreateCommandLine();
#ifdef _WIN32
    earlyCommandLine->InitFromString(GetCommandLineW());
#else
    earlyCommandLine->InitFromArgv(mainArgs.argc, mainArgs.argv);
#endif

    CefSettings settings;
    settings.windowless_rendering_enabled = true;
    settings.background_color = 0;
    settings.no_sandbox = true;

    if(earlyCommandLine->HasSwitch("home-path")) {
        CefString(&settings.root_cache_path).FromString(earlyCommandLine->GetSwitchValue("home-path"));
    }

    if(!CefInitialize(mainArgs, settings, app.get(), nullptr))
        return 0;

    struct CefShutdownScope {
        inline ~CefShutdownScope() {
            CefShutdown();
        }
    } shutdownScope;

    auto commandLine = CefCommandLine::GetGlobalCommandLine();

#ifdef _WIN32
    if(!commandLine->HasSwitch("webview-rpc-server")) {
        fprintf(stderr, "--webview-rpc-server was not specified\n");
        return 1;
    }

    WebViewRPCServerWindows rpcServer(commandLine->GetSwitchValue("webview-rpc-server").ToWString());
#else
    if(!commandLine->HasSwitch("webview-rpc-client-socket")) {
        fprintf(stderr, "--webview-rpc-client-socket was not specified\n");
        return 1;
    }

    WebViewRPCServerLinux rpcServer(strtoll(commandLine->GetSwitchValue("webview-rpc-client-socket").ToString().c_str(), nullptr, 10));
#endif

    CefRunMessageLoop();

    return 0;
}

#ifdef _WIN32
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    (void)hPrevInstance;
    (void)pCmdLine;
    (void)nCmdShow;

    CefMainArgs mainArgs(hInstance);

    return applicationMain(mainArgs);
}

#else

int main(int argc, char **argv) {
    CefMainArgs mainArgs(argc, argv);

    return applicationMain(mainArgs);
}

#endif


#include <WebView/CEFWebViewImplementation.h>

#include <windows.h>

static BOOL CALLBACK windowEnumerator(HWND hwnd, LPARAM lParam) {
    auto out = reinterpret_cast<HWND *>(lParam);

    DWORD processId;
    DWORD threadId = GetWindowThreadProcessId(hwnd, &processId);
    if(threadId == 0)
        return FALSE;

    static wchar_t requiredClassName[] = L"UnityWndClass";

    if(processId == GetCurrentProcessId()) {
        wchar_t className[sizeof(requiredClassName)];

        auto result = GetClassName(hwnd, className, sizeof(className));
        if(result != 0 && wcsncmp(requiredClassName, className, sizeof(className)) == 0) {

            *out = hwnd;
            return FALSE;
        }
    }

    return TRUE;
}

static HWND getWebViewWindow() {
    static HWND webViewWindow = nullptr;
    if(!webViewWindow) {
        EnumWindows(windowEnumerator, reinterpret_cast<LPARAM>(&webViewWindow));
    }

    if(!webViewWindow) {
        fprintf(stderr, "No Windows window suitable for the web view hosting was found\n");
    } else {
        printf("Using %p as the webview parent window\n", webViewWindow);
    }

    return webViewWindow;
}

float CEFWebViewImplementation::screenWidth() {
    RECT rect;
    if(GetClientRect(getWebViewWindow(), &rect)) {
        printf("Windows reports window size as (%d,%d)\n", rect.right, rect.bottom);
        return rect.right;
    } else {
        return 0.0f;
    }
}

float CEFWebViewImplementation::screenHeight() {
    RECT rect;
    if(GetClientRect(getWebViewWindow(), &rect)) {
        printf("Windows reports window size as (%d,%d)\n", rect.right, rect.bottom);
        return rect.bottom;
    } else {
        return 0.0f;
    }
}

int64_t CEFWebViewImplementation::getParentWindowHandle() {
    return reinterpret_cast<int64_t>(getWebViewWindow());
}


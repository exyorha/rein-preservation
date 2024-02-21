#include "ParentedContainerWindow.h"
#include "WebViewContainerDelegate.h"

#include <X11/X.h>

ParentedContainerWindow::ParentedContainerWindow(uintptr_t platformParentWindow) {
    m_window = CefWindow::CreateTopLevelWindow(CefRefPtr<WebViewContainerDelegate>(new WebViewContainerDelegate));


}

ParentedContainerWindow::~ParentedContainerWindow() = default;

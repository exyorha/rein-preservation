#ifndef PARENTED_CONTAINER_WINDOW_H
#define PARENTED_CONTAINER_WINDOW_H

#include <include/views/cef_window.h>

class ParentedContainerWindow {
public:
    explicit ParentedContainerWindow(uintptr_t platformParentWindow);
    ~ParentedContainerWindow();

    ParentedContainerWindow(const ParentedContainerWindow &other) = delete;
    ParentedContainerWindow &operator =(const ParentedContainerWindow &other) = delete;

    inline const CefRefPtr<CefWindow> &window() const {
        return m_window;
    }

private:
    CefRefPtr<CefWindow> m_window;
};

#endif

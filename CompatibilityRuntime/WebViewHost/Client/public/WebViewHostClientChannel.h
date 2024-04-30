#ifndef WEBVIEWHOSTCLIENTCHANNEL_H
#define WEBVIEWHOSTCLIENTCHANNEL_H

#ifdef _WIN32
#include "WebViewHostClientChannelWindows.h"

using WebViewHostClientChannel = WebViewHostClientChannelWindows;

#else
#include "WebViewHostClientChannelLinux.h"

using WebViewHostClientChannel = WebViewHostClientChannelLinux;

#endif

#endif

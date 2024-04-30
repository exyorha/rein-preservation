add_library(WebViewProtocol STATIC
    WebView/WebViewProtocol.proto
    Protocol/WebViewProtocolController.h
)

protobuf_generate(TARGET WebViewProtocol)
target_include_directories(WebViewProtocol PUBLIC ${CMAKE_CURRENT_BINARY_DIR} Protocol)
target_link_libraries(WebViewProtocol PUBLIC protobuf::libprotobuf)

if(OS_LINUX OR OS_WINDOWS)
  # Logical target used to link the libcef library on Linux and Windows.
  # On macOS the CEF framework is loaded dynamically at startup.
  ADD_LOGICAL_TARGET("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")
endif()

add_executable(WebViewHost WIN32
  main.cpp
  ParentedContainerWindow.cpp
  ParentedContainerWindow.h
  WebView.cpp
  WebView.h
  WebViewApp.cpp
  WebViewApp.h
  WebViewBrowserViewDelegate.cpp
  WebViewBrowserViewDelegate.h
  WebViewClient.cpp
  WebViewClient.h
  WebViewContainerDelegate.cpp
  WebViewContainerDelegate.h
  WebViewRPCImplementation.cpp
  WebViewRPCImplementation.h
  WebViewRPCServer.cpp
  WebViewRPCServer.h
  WebViewRPCService.cpp
  WebViewRPCService.h
)


if(WIN32)
  target_sources(WebViewHost PRIVATE
    WebViewRPCServerWindows.cpp
    WebViewRPCServerWindows.h
  )
  target_link_options(WebViewHost PRIVATE -mwindows -municode -static -static-libgcc)
else()
  target_sources(WebViewHost PRIVATE
    WebViewRPCServerLinux.cpp
    WebViewRPCServerLinux.h
  )
endif()

set_source_files_properties(WebViewRPCService.cpp WebViewRPCImplementation.cpp PROPERTIES COMPILE_OPTIONS -fexceptions)

SET_EXECUTABLE_TARGET_PROPERTIES(WebViewHost)

add_dependencies(WebViewHost libcef_dll_wrapper)

target_link_libraries(WebViewHost PRIVATE libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS} WebViewProtocol)

add_library(WebViewHostClient STATIC
  Client/public/WebViewHostClient.h
  Client/WebViewHostClient.cpp

  Client/public/WebViewHostClientChannel.h

  Client/public/WebViewHostClientConfiguration.h
  Client/WebViewHostClientConfiguration.cpp
)
target_include_directories(WebViewHostClient PUBLIC Client/public)
target_link_libraries(WebViewHostClient PUBLIC WebViewProtocol)

if(WIN32)
  set(webview_directory "webview/windows")
  target_sources(WebViewHostClient PRIVATE
    Client/public/WebViewHostClientChannelWindows.h
    Client/WebViewHostClientChannelWindows.cpp
  )
  target_compile_definitions(WebViewHostClient PRIVATE -DUNICODE -D_UNICODE -DWIN32_LEAN_AND_MEAN -DNOMINMAX)
else()
  set(webview_directory "webview/linux")
  target_sources(WebViewHostClient PRIVATE
    Client/public/WebViewHostClientChannelLinux.h
    Client/WebViewHostClientChannelLinux.cpp
  )
endif()

if(NOT WIN32)
  set_target_properties(WebViewHost PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "\$ORIGIN"
  )
endif()

install(TARGETS WebViewHost DESTINATION ${webview_directory} COMPONENT GameAssembly)

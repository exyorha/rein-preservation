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

add_executable(WebViewHost
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
set_source_files_properties(WebViewRPCService.cpp WebViewRPCImplementation.cpp PROPERTIES COMPILE_OPTIONS -fexceptions)

SET_EXECUTABLE_TARGET_PROPERTIES(WebViewHost)

add_dependencies(WebViewHost libcef_dll_wrapper)

target_link_libraries(WebViewHost PRIVATE libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS} WebViewProtocol)

add_library(WebViewHostClient STATIC
  Client/public/WebViewHostClient.h
  Client/WebViewHostClient.cpp
)
target_include_directories(WebViewHostClient PUBLIC Client/public)
target_link_libraries(WebViewHostClient PUBLIC WebViewProtocol)

if(WIN32)
  set(webview_directory "webview/windows")
else()
  set(webview_directory "webview/linux")
endif()

if(NOT WIN32)
  set_target_properties(WebViewHost PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "\$ORIGIN"
  )
endif()

install(TARGETS WebViewHost DESTINATION ${webview_directory} COMPONENT GameAssembly)

list(TRANSFORM CEF_BINARY_FILES PREPEND "${CEF_BINARY_DIR_RELEASE}/" OUTPUT_VARIABLE cef_binaries)
install(FILES ${cef_binaries} DESTINATION ${webview_directory} COMPONENT GameAssembly)

list(TRANSFORM CEF_RESOURCE_FILES PREPEND "${CEF_RESOURCE_DIR}/" OUTPUT_VARIABLE cef_resources)
list(REMOVE_ITEM cef_resources "${CEF_RESOURCE_DIR}/locales")
install(FILES ${cef_resources} DESTINATION ${webview_directory} COMPONENT GameAssembly)
install(FILES "${CEF_RESOURCE_DIR}/locales/en-US.pak" DESTINATION "${webview_directory}/locales" COMPONENT GameAssembly)

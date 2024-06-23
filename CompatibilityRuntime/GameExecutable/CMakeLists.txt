add_executable(GameExecutable WIN32
    AndroidJNI.cpp

    CommonUtilities.cpp
    CommonUtilities.h

    GameEntryPoint.h

    GameServerInterface.cpp
    GameServerInterface.h


    GLES/GLESAPISet.cpp
    GLES/GLESAPISet.h
    GLES/GLESObjectHandle.h
    GLES/GLESRenderingOverlay.cpp
    GLES/GLESRenderingOverlay.h

    Input/TouchEmulator.cpp
    Input/TouchEmulator.h

    Input/TouchInputAreaReceiver.cpp
    Input/TouchInputAreaReceiver.h

    Input/TouchInputReceiver.cpp
    Input/TouchInputReceiver.h

    Input/UnityTouchInputReceiver.cpp
    Input/UnityTouchInputReceiver.h

    main.cpp

    Input.cpp
    Input.h

    Java/AdjustSdkAdjust.cpp
    Java/AdjustSdkAdjust.h
    Java/AndroidContext.cpp
    Java/AndroidContext.h
    Java/AndroidUtilLog.cpp
    Java/AndroidUtilLog.h
    Java/JavaClasses.cpp
    Java/JNIAccessibleObject.cpp
    Java/JNIAccessibleObject.h
    Java/JNIBasePrimitiveArray.h
    Java/JNIClass.cpp
    Java/JNIClass.h
    Java/JNIConstructor.cpp
    Java/JNIConstructor.h
    Java/JNIExecutable.cpp
    Java/JNIExecutable.h
    Java/JNIField.cpp
    Java/JNIFloatArray.cpp
    Java/JNIFloatArray.h
    Java/JNIGlobalState.cpp
    Java/JNIGlobalState.h
    Java/JNIMethod.cpp
    Java/JNIMethod.h
    Java/JNIMethodInvocation.h
    Java/JNIObject.cpp
    Java/JNIObject.h
    Java/JNIState.cpp
    Java/JNIState.h
    Java/JNIString.cpp
    Java/JNIString.h
    Java/JNISystem.cpp
    Java/JNISystem.h
    Java/JNIThrowable.cpp
    Java/JNIThrowable.h
    Java/JNIUnityPlayer.cpp
    Java/JNIUnityPlayer.h
    Java/JNIUnityReflectionHelper.cpp
    Java/JNIUnityReflectionHelper.h
    Java/JValue.cpp
    Java/JValue.h
    Java/LiliumDefaultAttestCallback.cpp
    Java/LiliumDefaultAttestCallback.h
    Java/NativeGallery.cpp
    Java/NativeGallery.h
    Java/NativeGalleryPermissionReceiver.cpp
    Java/NativeGalleryPermissionReceiver.h
    Java/UniWebViewInterface.cpp
    Java/UniWebViewInterface.h

    Octo.cpp
    Octo.h

    ProcessCommandLine.h

    WebView/WebViewImplementation.cpp
    WebView/WebViewImplementation.h
    WebView/WebViewImplementationFactory.cpp
    WebView/WebViewImplementationFactory.h

    UnityPatches.cpp
    UnityPatches.h

    UnityWebRequest.cpp

    WebSupport.cpp
    WebSupport.h

)
if(NOT WIN32)
    target_sources(GameExecutable PRIVATE
        EntryPointLinux.cpp

        GLES/SDL/RealSDLSymbols.cpp
        GLES/SDL/RealSDLSymbols.h
        GLES/SDL/SDLWrapper.cpp
        GLES/SDL/SDLWrapper.h

        ProcessCommandLineLinux.cpp
        ProcessCommandLineLinux.h
    )

endif()

if(WIN32)
    target_sources(GameExecutable PRIVATE
        GLES/WGL/WGLHooking.cpp
        GLES/WGL/WGLHooking.h

        GLES/WGL/WindowsImportRedirection.cpp
        GLES/WGL/WindowsImportRedirection.h

        EntryPointWindows.cpp

        ProcessCommandLineWindows.cpp
        ProcessCommandLineWindows.h

        WindowsHelpers.cpp
        WindowsHelpers.h

        GameExecutable.rc
    )
    target_compile_definitions(GameExecutable PRIVATE -DWIN32_LEAN_AND_MEAN -DUNICODE -D_UNICODE -DNOMINMAX)
    target_link_libraries(GameExecutable PRIVATE opengl32 ws2_32)
    set_source_files_properties(GameExecutable.rc PROPERTIES COMPILE_OPTIONS -c65001)
endif()

target_include_directories(GameExecutable PRIVATE . ${CMAKE_CURRENT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/thirdparty/khronos-headers)
target_link_libraries(GameExecutable PRIVATE GameAssembly Il2CppUtilities ClientDataAccess gitversion)
set_target_properties(GameExecutable PROPERTIES
    OUTPUT_NAME NieR
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "\$ORIGIN"
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)

if(NOT WIN32)
    target_link_libraries(GameExecutable PRIVATE UnityStub)
endif()

if(UNIX)
    target_link_options(GameExecutable PRIVATE -Wl,-z,now "-Wl,--export-dynamic-symbol=SDL_*" "-Wl,--no-copy-dt-needed-entries"
        -static-libstdc++
        -Wl,--hash-style=sysv)
endif()

if(MINGW)
    target_link_options(GameExecutable PRIVATE -Wl,-Bstatic -static-libgcc -municode)
endif()

if(TARGET WebViewHostClient)
    target_sources(GameExecutable PRIVATE
        WebView/CEFCompositor.cpp
        WebView/CEFCompositor.h
        WebView/CEFSurface.cpp
        WebView/CEFSurface.h
        WebView/CEFSurfaceInputReceiver.cpp
        WebView/CEFSurfaceInputReceiver.h
        WebView/CEFWebViewImplementation.cpp
        WebView/CEFWebViewImplementation.h
    )
    target_compile_definitions(GameExecutable PRIVATE -DBUILDING_WITH_CEF)
    target_link_libraries(GameExecutable PRIVATE WebViewHostClient)
endif()

if(TARGET PkgConfig::MPV)
    target_sources(GameExecutable PRIVATE
        VideoPlayer/AVProVideoNativeBypass.cpp
        VideoPlayer/AVProVideoNativeBypass.h

        VideoPlayer/Java/AVProMobileVideo.cpp
        VideoPlayer/Java/AVProMobileVideo.h
        VideoPlayer/Java/AVProVideoPlayer.cpp
        VideoPlayer/Java/AVProVideoPlayer.h

        VideoPlayer/MPVBasePropertyObserver.cpp
        VideoPlayer/MPVBasePropertyObserver.h

        VideoPlayer/MPVError.cpp
        VideoPlayer/MPVError.h

        VideoPlayer/MPVPlayer.cpp
        VideoPlayer/MPVPlayer.h

        VideoPlayer/MPVTypes.h
    )

    target_compile_definitions(GameExecutable PRIVATE -DBUILDING_WITH_MPV)
    target_link_libraries(GameExecutable PRIVATE PkgConfig::MPV)
endif()

install(TARGETS GameExecutable DESTINATION . COMPONENT GameAssembly)

separate_unix_debug_info(GameExecutable DESTINATION . COMPONENT GameAssembly)

add_executable(GameExecutable
    AndroidJNI.cpp

    CommonUtilities.cpp
    CommonUtilities.h

    FastAES.h

    GameEntryPoint.h

    GameServerInterface.cpp
    GameServerInterface.h

    GLES/BaseGLESContext.cpp
    GLES/BaseGLESContext.h
    GLES/GLESGlobalSwitches.h
    GLES/GLESImplementationType.h

    GLES/ANGLE/ANGLELibrary.cpp
    GLES/ANGLE/ANGLELibrary.h
    GLES/ANGLE/EGLWindowSurface.cpp
    GLES/ANGLE/EGLWindowSurface.h
    GLES/ANGLE/InitializedEGLContext.cpp
    GLES/ANGLE/InitializedEGLContext.h
    GLES/ANGLE/InitializedEGLDisplay.cpp
    GLES/ANGLE/InitializedEGLDisplay.h
    GLES/ANGLE/PlatformDynamicLibrary.h

    GLES/Shim/GLESContextShim.cpp
    GLES/Shim/GLESContextShim.h
    GLES/Shim/ShimExtensionString.cpp
    GLES/Shim/ShimExtensionString.h
    GLES/Shim/ShimNextContextSymbols.cpp
    GLES/Shim/ShimNextContextSymbols.h

    GLES/TextureEmulation/EmulatedTextureFormat.cpp
    GLES/TextureEmulation/EmulatedTextureFormat.h
    GLES/TextureEmulation/astc_dec/astc_decomp.cpp
    GLES/TextureEmulation/astc_dec/astc_decomp.h
    GLES/TextureEmulation/stb_dxt.cpp
    GLES/TextureEmulation/stb_dxt.h

    main.cpp

    Input.cpp
    Input.h

    Java/AdjustSdkAdjust.cpp
    Java/AdjustSdkAdjust.h
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
    Java/UniWebViewInterface.cpp
    Java/UniWebViewInterface.h

    Octo.cpp
    Octo.h
    OctoContentStorage.cpp
    OctoContentStorage.h

    Octo/Proto/Database.proto

    ProcessCommandLine.h

    WebView/WebViewImplementation.cpp
    WebView/WebViewImplementation.h
    WebView/WebViewImplementationFactory.cpp
    WebView/WebViewImplementationFactory.h

    UnityPatches.cpp
    UnityPatches.h

    UnityWebRequest.cpp

)

protobuf_generate(TARGET GameExecutable)

if(NOT WIN32)
    target_sources(GameExecutable PRIVATE
        EntryPointLinux.cpp

        GLES/SDL/RealSDLSymbols.cpp
        GLES/SDL/RealSDLSymbols.h
        GLES/SDL/SDLGLESImplementation.cpp
        GLES/SDL/SDLGLESImplementation.h
        GLES/SDL/SDLGLESImplementationANGLE.cpp
        GLES/SDL/SDLGLESImplementationANGLE.h
        GLES/SDL/SDLGLESImplementationNative.cpp
        GLES/SDL/SDLGLESImplementationNative.h
        GLES/SDL/SDLWrapper.cpp
        GLES/SDL/SDLWrapper.h
        GLES/SDL/SDLWrapperAttributeSet.cpp
        GLES/SDL/SDLWrapperAttributeSet.h

        GLES/ANGLE/LinuxDynamicLibrary.cpp
        GLES/ANGLE/LinuxDynamicLibrary.h

        ProcessCommandLineLinux.cpp
        ProcessCommandLineLinux.h
    )

endif()

if(WIN32)
    target_sources(GameExecutable PRIVATE
        EntryPointWindows.cpp

        FastAESWindows.cpp

        ProcessCommandLineWindows.cpp
        ProcessCommandLineWindows.h

        WindowsHelpers.cpp
        WindowsHelpers.h

        GLES/ANGLE/WindowsDynamicLibrary.cpp
        GLES/ANGLE/WindowsDynamicLibrary.h

        GLES/WGL/ContextAndSurfacePair.cpp
        GLES/WGL/ContextAndSurfacePair.h
        GLES/WGL/DCAssociatedWindowSurface.cpp
        GLES/WGL/DCAssociatedWindowSurface.h
        GLES/WGL/WGLHooking.cpp
        GLES/WGL/WGLHooking.h
        GLES/WGL/WGLImplementation.cpp
        GLES/WGL/WGLImplementation.h
        GLES/WGL/WGLImplementationANGLE.cpp
        GLES/WGL/WGLImplementationANGLE.h
        GLES/WGL/WGLImplementationNative.cpp
        GLES/WGL/WGLImplementationNative.h
        GLES/WGL/WindowsImportRedirection.cpp
        GLES/WGL/WindowsImportRedirection.h
    )
    target_compile_definitions(GameExecutable PRIVATE -DWIN32_LEAN_AND_MEAN -DUNICODE -D_UNICODE -DNOMINMAX)
    target_link_libraries(GameExecutable PRIVATE bcrypt opengl32 ws2_32)
else()
    target_sources(GameExecutable PRIVATE
        FastAESOpenSSL.cpp
    )

    target_link_libraries(GameExecutable PRIVATE OpenSSL::Crypto)
endif()

target_include_directories(GameExecutable PRIVATE . ${CMAKE_CURRENT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/thirdparty/khronos-headers)
target_link_libraries(GameExecutable PRIVATE UnityStub GameAssembly protobuf::libprotobuf Il2CppUtilities)
set_target_properties(GameExecutable PROPERTIES
    OUTPUT_NAME NieR
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "\$ORIGIN"
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)

if(UNIX)
    target_link_options(GameExecutable PRIVATE -Wl,-z,now "-Wl,--export-dynamic-symbol=SDL_*" "-Wl,--no-copy-dt-needed-entries")
endif()

if(MINGW)
    target_link_options(GameExecutable PRIVATE -Wl,-Bstatic -static-libgcc -municode)
endif()

if(TARGET WebViewHostClient)
    target_sources(GameExecutable PRIVATE
        WebView/CEFWebViewImplementation.cpp
        WebView/CEFWebViewImplementation.h

        WebView/CEFWebViewWindowing.cpp
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

        VideoPlayer/VideoRenderingOpenGLAPISet.cpp
        VideoPlayer/VideoRenderingOpenGLAPISet.h
    )

    target_compile_definitions(GameExecutable PRIVATE -DBUILDING_WITH_MPV)
    target_link_libraries(GameExecutable PRIVATE PkgConfig::MPV)
endif()

install(TARGETS GameExecutable DESTINATION . COMPONENT GameAssembly)
install(FILES app.info DESTINATION $<TARGET_FILE_BASE_NAME:GameExecutable>_Data COMPONENT GameAssembly)

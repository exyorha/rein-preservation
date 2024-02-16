add_executable(GameExecutable
    AndroidJNI.cpp

    CommonUtilities.cpp
    CommonUtilities.h

    FastAES.h

    GameEntryPoint.h

    GLES/BaseGLESContext.cpp
    GLES/BaseGLESContext.h
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

    main.cpp

    Il2CppUtilities.cpp
    Il2CppUtilities.h

    Input.cpp
    Input.h

    Java/AdjustSdkAdjust.cpp
    Java/AdjustSdkAdjust.h
    Java/AndroidUtilLog.cpp
    Java/AndroidUtilLog.h
    Java/JavaClasses.cpp
    Java/JNIAccessibleObject.cpp
    Java/JNIAccessibleObject.h
    Java/JNIClass.cpp
    Java/JNIClass.h
    Java/JNIField.cpp
    Java/JNIGlobalState.cpp
    Java/JNIGlobalState.h
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
    Java/LiliumDefaultAttestCallback.cpp
    Java/LiliumDefaultAttestCallback.h
    Java/UniWebViewInterface.cpp
    Java/UniWebViewInterface.h

    Octo.cpp
    Octo.h
    OctoContentStorage.cpp
    OctoContentStorage.h

    Octo/Proto/Database.proto

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
    )

endif()

if(WIN32)
    target_sources(GameExecutable PRIVATE
        EntryPointWindows.cpp

        FastAESWindows.cpp
        Il2CppUtilitiesWindows.cpp
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
    target_link_libraries(GameExecutable PRIVATE bcrypt opengl32)
else()
    target_sources(GameExecutable PRIVATE
        FastAESOpenSSL.cpp
        Il2CppUtilitiesIconv.cpp
    )

    target_link_libraries(GameExecutable PRIVATE Iconv::Iconv OpenSSL::Crypto)
endif()

target_include_directories(GameExecutable PRIVATE . ${CMAKE_CURRENT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/thirdparty/khronos-headers)
target_link_libraries(GameExecutable PRIVATE UnityStub GameAssembly protobuf::libprotobuf)
set_target_properties(GameExecutable PROPERTIES
    OUTPUT_NAME NieR
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "\$ORIGIN"
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)

if(UNIX)
    target_link_options(GameExecutable PRIVATE -Wl,-z,now "-Wl,--export-dynamic-symbol=SDL_*")
endif()

if(MINGW)
    target_link_options(GameExecutable PRIVATE -Wl,-Bstatic -static-libgcc -municode)
endif()

install(TARGETS GameExecutable DESTINATION . COMPONENT GameAssembly)
install(FILES app.info DESTINATION $<TARGET_FILE_BASE_NAME:GameExecutable>_Data COMPONENT GameAssembly)

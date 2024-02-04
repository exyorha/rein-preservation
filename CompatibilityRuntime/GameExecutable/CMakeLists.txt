add_executable(GameExecutable
    AndroidJNI.cpp

    FastAES.cpp
    FastAES.h

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

    main.cpp

    Octo.cpp
    Octo.h
    OctoContentStorage.cpp
    OctoContentStorage.h

    UnityWebRequest.cpp

    Octo/Proto/Database.proto
)

protobuf_generate(TARGET GameExecutable)

target_include_directories(GameExecutable PRIVATE . ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(GameExecutable PRIVATE UnityStub GameAssembly Iconv::Iconv GameServer OpenSSL::Crypto protobuf::libprotobuf)
set_target_properties(GameExecutable PROPERTIES
    OUTPUT_NAME NieR
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "\$ORIGIN"
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)

if(UNIX)
    target_link_options(GameExecutable PRIVATE -Wl,-z,now)
endif()

install(TARGETS GameExecutable DESTINATION . COMPONENT GameAssembly)
install(FILES app.info DESTINATION $<TARGET_FILE_BASE_NAME:GameExecutable>_Data COMPONENT GameAssembly)

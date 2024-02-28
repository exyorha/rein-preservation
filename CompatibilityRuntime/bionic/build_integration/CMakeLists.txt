set(ANDROID_NDK_ROOT "" CACHE PATH "Path to the Android NDK")
if(NOT ANDROID_NDK_ROOT)
    message(SEND_ERROR "ANDROID_NDK_ROOT must be specified with the path to the installed Android NDK for building Bionic")
endif()

execute_process(
    COMMAND
        ${CMAKE_COMMAND}
        -S ${CMAKE_CURRENT_SOURCE_DIR}/..
        -B ${CMAKE_CURRENT_BINARY_DIR}/bionic_build
        -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK_ROOT}/build/cmake/android.toolchain.cmake
        -DANDROID_ABI=arm64-v8a
        -DANDROID_PLATFORM=android-21
        -DCMAKE_BUILD_TYPE=Release
    COMMAND_ERROR_IS_FATAL ANY
)

add_custom_target(bionic_build_final ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/bionic.so.pe
    VERBATIM
)

add_custom_target(bionic_build ALL
    COMMAND
        ${CMAKE_COMMAND}
        --build ${CMAKE_CURRENT_BINARY_DIR}/bionic_build
    BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/bionic_build/bionic.so
    VERBATIM
)

add_dependencies(bionic_build_final bionic_build makeelfpe)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/bionic.so.pe
    COMMAND $<TARGET_FILE:makeelfpe> ${CMAKE_CURRENT_BINARY_DIR}/bionic_build/bionic.so ${CMAKE_CURRENT_BINARY_DIR}/bionic.so.pe
    DEPENDS $<TARGET_FILE:makeelfpe> ${CMAKE_CURRENT_BINARY_DIR}/bionic_build/bionic.so
    VERBATIM
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/bionic.so.pe DESTINATION . COMPONENT GameAssembly)

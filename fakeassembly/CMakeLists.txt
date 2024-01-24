cmake_minimum_required(VERSION 3.20)
project(il2cpp-arm-adapter)

include(GenerateExportHeader)

find_package(PkgConfig REQUIRED)
pkg_check_modules(FFI REQUIRED IMPORTED_TARGET libffi)

set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

set(DYNARMIC_USE_BUNDLED_EXTERNALS TRUE CACHE BOOL "Use all bundled externals (useful when e.g. cross-compiling)" FORCE)
add_subdirectory(dynarmic)

find_program(RUBY ruby REQUIRED)

enable_language(ASM)

add_library(GameAssembly SHARED
    il2cpp_api/il2cpp-api-functions.h
    il2cpp_api/il2cpp-api-types.h
    il2cpp_api/il2cpp-api.h
    il2cpp_api/il2cpp-blob.h
    il2cpp_api_replacements/il2cpp-config-api.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/il2cpp_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/apifuncs.cpp
    android_api.cpp
    android_api.h
    ElfModule.cpp
    ElfModule.h
    FileDescriptor.cpp
    FileDescriptor.h
    icall_api.cpp
    Image.cpp
    Image.h
    JIT.cpp
    JIT.h
    JITThreadContext.cpp
    JITThreadContext.h
    signal_compatibility.cpp
    signal_compatibility.h
    support.h
    support.cpp
    thunking.cpp
    thunking.h
    ThunkManager.cpp
    ThunkManager.h
    thunkutils.S
    SystemAPIThunking.cpp
    SystemAPIThunking.h
)
target_include_directories(GameAssembly PUBLIC il2cpp_api_replacements il2cpp_api ${CMAKE_CURRENT_BINARY_DIR}/include)
target_include_directories(GameAssembly PRIVATE .)
target_link_libraries(GameAssembly PRIVATE dynarmic PkgConfig::FFI)

set_target_properties(GameAssembly PROPERTIES
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN TRUE
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)

if(UNIX)
    target_link_options(GameAssembly PRIVATE -Wl,-z,defs "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/version.lds")
    set_target_properties(GameAssembly PROPERTIES LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/version.lds")
endif()

GENERATE_EXPORT_HEADER(
    GameAssembly
    BASE_NAME il2cpp
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/include/il2cpp_export.h
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/apifuncs.cpp
    COMMAND
        ${RUBY} --
        ${CMAKE_CURRENT_SOURCE_DIR}/makeapifuncs.rb
        ${CMAKE_CURRENT_SOURCE_DIR}/il2cpp_api/il2cpp-api-functions.h
        ${CMAKE_CURRENT_BINARY_DIR}/apifuncs.cpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/makeapifuncs.rb
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/il2cpp_api/il2cpp-api-functions.h
    VERBATIM
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/armlib/armlib.so
    COMMAND
        aarch64-linux-gnu-gcc
        -shared
        -o ${CMAKE_CURRENT_BINARY_DIR}/armlib/armlib.so
        ${CMAKE_CURRENT_SOURCE_DIR}/armlib/armlib.c
        -x assembler-with-cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/armlib/setjmp.S
        -nostdinc
        -nostdlib
        -ffreestanding
        -Wall -W
        -O2
        -fPIC
        -Wl,-Bsymbolic
        -Wl,--hash-style=sysv
        -Wl,-z,norelro
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/armlib/armlib.c
        ${CMAKE_CURRENT_SOURCE_DIR}/armlib/setjmp.S
        ${CMAKE_CURRENT_SOURCE_DIR}/armlib/nanoprintf.h
)

add_custom_target(armlib DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/armlib/armlib.so)
add_dependencies(GameAssembly armlib)

add_subdirectory(interrogator)

install(TARGETS GameAssembly interrogator DESTINATION . COMPONENT GameAssembly)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/armlib/armlib.so DESTINATION . COMPONENT GameAssembly)
install(FILES app.info DESTINATION LinuxPlayer_Data COMPONENT GameAssembly)

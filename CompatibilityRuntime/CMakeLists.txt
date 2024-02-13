cmake_minimum_required(VERSION 3.20)

project(CompatibilityRuntime)

add_subdirectory(UnityStub)

set(prev ${CMAKE_POSITION_INDEPENDENT_CODE})
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

set(DYNARMIC_USE_BUNDLED_EXTERNALS TRUE CACHE BOOL "Use all bundled externals (useful when e.g. cross-compiling)" FORCE)
add_subdirectory(thirdparty/dynarmic)

set(CMAKE_POSITION_INDEPENDENT_CODE ${prev})

find_program(RUBY ruby REQUIRED)

enable_language(ASM)

find_library(FFI_LIBRARY ffi REQUIRED)
find_path(FFI_INCLUDEPATH ffi.h REQUIRED)

add_library(ffi INTERFACE)
target_link_libraries(ffi INTERFACE ${FFI_LIBRARY})
target_include_directories(ffi INTERFACE ${FFI_INCLUDEPATH})

find_package(Protobuf CONFIG REQUIRED)

if(WIN32 AND NOT TARGET protobuf::protoc)
    message(STATUS "Trying to find the host 'protoc'")

    find_program(HOST_PROTOC protoc REQUIRED)
    add_executable(protobuf::protoc IMPORTED)
    set_target_properties(protobuf::protoc PROPERTIES IMPORTED_LOCATION "${HOST_PROTOC}")
endif()

if(NOT WIN32)
    find_package(gRPC CONFIG REQUIRED)
    find_package(Iconv REQUIRED)
    find_package(OpenSSL REQUIRED COMPONENTS Crypto)

endif()

add_subdirectory(GameFramework)

add_subdirectory(GameExecutable)


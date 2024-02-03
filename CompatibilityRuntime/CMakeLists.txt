cmake_minimum_required(VERSION 3.20)

project(CompatibilityRuntime)

find_package(PkgConfig REQUIRED)
pkg_check_modules(FFI REQUIRED IMPORTED_TARGET libffi)

find_package(Iconv REQUIRED)

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(OpenSSL REQUIRED COMPONENTS Crypto)

set(prev ${CMAKE_POSITION_INDEPENDENT_CODE})
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

set(DYNARMIC_USE_BUNDLED_EXTERNALS TRUE CACHE BOOL "Use all bundled externals (useful when e.g. cross-compiling)" FORCE)
add_subdirectory(thirdparty/dynarmic)

set(CMAKE_POSITION_INDEPENDENT_CODE ${prev})

find_program(RUBY ruby REQUIRED)

enable_language(ASM)

add_subdirectory(armlib)
add_subdirectory(UnityStub)
add_subdirectory(GameFramework)
add_subdirectory(GameServer)
add_subdirectory(GameExecutable)

add_subdirectory(interrogator)

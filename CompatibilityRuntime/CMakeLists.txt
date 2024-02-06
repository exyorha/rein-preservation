cmake_minimum_required(VERSION 3.20)

project(CompatibilityRuntime)

find_package(PkgConfig REQUIRED)
pkg_check_modules(FFI REQUIRED IMPORTED_TARGET libffi)
pkg_check_modules(LZ4 REQUIRED IMPORTED_TARGET liblz4)

find_package(Iconv REQUIRED)

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(OpenSSL REQUIRED COMPONENTS Crypto)
find_package(SQLite3 REQUIRED)

set(prev ${CMAKE_POSITION_INDEPENDENT_CODE})
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

set(DYNARMIC_USE_BUNDLED_EXTERNALS TRUE CACHE BOOL "Use all bundled externals (useful when e.g. cross-compiling)" FORCE)
add_subdirectory(thirdparty/dynarmic)

set(CMAKE_POSITION_INDEPENDENT_CODE ${prev})

find_program(RUBY ruby REQUIRED)

enable_language(ASM)

set(MSGPACK_CXX20 TRUE CACHE BOOL "Using c++20 compiler" FORCE)
set(MSGPACK_USE_BOOST FALSE CACHE BOOL "Use Boost libraried" FORCE)
set(MSGPACK_BUILD_DOCS FALSE CACHE BOOL "Build Doxygen documentation" FORCE)
add_subdirectory(thirdparty/msgpack-c)

add_subdirectory(armlib)
add_subdirectory(UnityStub)
add_subdirectory(GameFramework)
add_subdirectory(master_database_ext)
add_subdirectory(GameServer)
add_subdirectory(GameExecutable)

add_subdirectory(interrogator)

cmake_minimum_required(VERSION 3.20)

project(CompatibilityRuntime)

add_subdirectory(UnityStub)

set(prev ${CMAKE_POSITION_INDEPENDENT_CODE})
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

set(DYNARMIC_USE_BUNDLED_EXTERNALS TRUE CACHE BOOL "Use all bundled externals (useful when e.g. cross-compiling)" FORCE)
add_subdirectory(thirdparty/dynarmic)

set(CMAKE_POSITION_INDEPENDENT_CODE ${prev})

find_program(RUBY ruby REQUIRED)

if(CEF_ROOT)
    message(STATUS "CEF_ROOT specified as ${CEF_ROOT}, trying to find CEF")
    list(APPEND CMAKE_MODULE_PATH "${CEF_ROOT}/cmake")

    set(USE_SANDBOX OFF CACHE BOOL "Enable or disable use of the sandbox." FORCE)
    set(USE_ATL OFF CACHE BOOL "Enable or disable use of ATL." FORCE)
    find_package(CEF REQUIRED)
endif()

if(NOT CEF_ROOT)
    message(STATUS "CEF wasn't found or the CEF location wasn't configured, will build without webview support")
endif()

enable_language(ASM)

find_library(FFI_LIBRARY ffi REQUIRED)
find_path(FFI_INCLUDEPATH ffi.h REQUIRED)

add_library(ffi INTERFACE)
target_link_libraries(ffi INTERFACE ${FFI_LIBRARY})
target_include_directories(ffi INTERFACE ${FFI_INCLUDEPATH})

find_package(PkgConfig)

if(PkgConfig_FOUND)
    pkg_check_modules(MPV IMPORTED_TARGET mpv)
endif()

if(NOT TARGET PkgConfig::MPV)
    message(STATUS "mpv was not found, will build without video playback (cutscene) support")
endif()

if(NOT WIN32)
    find_package(Iconv REQUIRED)
endif()

add_subdirectory(makeelfpe)

if(CEF_ROOT)
    add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)
    add_subdirectory(WebViewHost)
endif()

add_subdirectory(Il2CppUtilities)

add_subdirectory(GameFramework)

add_subdirectory(GameExecutable)

if(NOT WIN32)
    add_subdirectory(interrogator)
endif()

if(NOT CMAKE_CROSSCOMPILING)
    add_subdirectory(bionic/build_integration)
endif()

add_subdirectory(Launcher)

cmake_minimum_required(VERSION 3.20)
project(rein-asset-processing)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MINIZIP REQUIRED IMPORTED_TARGET minizip)
pkg_check_modules(LZ4 REQUIRED IMPORTED_TARGET liblz4)

add_subdirectory(unityasset)

add_executable(apk2gamedir
    asset_reprocessing.cpp
    asset_reprocessing.h
    apk2gamedir.cpp
    conversion_context.cpp
    conversion_context.h
    unzip_handle.cpp
    unzip_handle.h
    )
target_link_libraries(apk2gamedir PRIVATE PkgConfig::MINIZIP UnityAsset PkgConfig::LZ4)

set_target_properties(apk2gamedir PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED TRUE)

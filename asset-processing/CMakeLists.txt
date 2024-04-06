cmake_minimum_required(VERSION 3.20)
project(rein-asset-processing)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MINIZIP REQUIRED IMPORTED_TARGET minizip)

add_subdirectory(unityasset)

add_executable(apk2gamedir apk2gamedir.cpp unzip_handle.cpp unzip_handle.h)
target_link_libraries(apk2gamedir PRIVATE PkgConfig::MINIZIP UnityAsset)

set_target_properties(apk2gamedir PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED TRUE)

cmake_minimum_required(VERSION 3.20)
project(rein-asset-processing)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MINIZIP REQUIRED IMPORTED_TARGET minizip)
pkg_check_modules(LZ4 REQUIRED IMPORTED_TARGET liblz4)

add_subdirectory(unityasset)

add_executable(process_assets
    astc_dec/astc_decomp.cpp
    astc_dec/astc_decomp.h

    crunch/crn_decomp.h
    crunch/crn_defs.h
    crunch/crnlib.h

    etc2_transcoder.cpp
    etc2_transcoder.h

    HalfFloat.cpp
    HalfFloat.h

    rgbcx.cpp
    rgbcx.h
    rgbcx_table4.h

    asset_reprocessing.cpp
    asset_reprocessing.h
    conversion_context.cpp
    conversion_context.h
    process_assets.cpp
    unzip_handle.cpp
    unzip_handle.h
    )
target_link_libraries(process_assets PRIVATE PkgConfig::MINIZIP UnityAsset PkgConfig::LZ4)

set_target_properties(process_assets PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED TRUE)

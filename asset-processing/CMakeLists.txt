cmake_minimum_required(VERSION 3.20)
project(rein-asset-processing)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MINIZIP REQUIRED IMPORTED_TARGET minizip)

option(ENABLE_BC7E "Use bc7e for BC7 encoding, requiring ISPC. If disabled, bc7enc will be used, without such requirement." OFF)

add_executable(process_assets
    astc_dec/astc_decomp.cpp
    astc_dec/astc_decomp.h

    crunch/crn_decomp.h
    crunch/crn_defs.h
    crunch/crnlib.h

    asset_reprocessing.cpp
    asset_reprocessing.h
    conversion_context.cpp
    conversion_context.h

    etc2_transcoder.cpp
    etc2_transcoder.h

    HalfFloat.cpp
    HalfFloat.h

    process_assets.cpp
    unzip_handle.cpp
    unzip_handle.h
)

if(ENABLE_BC7E)
    enable_language(ISPC)
    target_sources(process_assets PRIVATE bc7enc_rdo/bc7e.ispc)
    set_source_files_properties(bc7enc_rdo/bc7e.ispc PROPERTIES COMPILE_FLAGS --pic)
    target_compile_definitions(process_assets PRIVATE -DUSE_BC7E)
else()
    target_sources(process_assets PRIVATE bc7enc_rdo/bc7enc.cpp)
endif()

target_link_libraries(process_assets PRIVATE PkgConfig::MINIZIP UnityAsset lz4 ClientDataAccess ZipFileWriter)
target_include_directories(process_assets PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

set_target_properties(process_assets PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED TRUE)

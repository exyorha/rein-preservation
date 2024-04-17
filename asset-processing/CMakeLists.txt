cmake_minimum_required(VERSION 3.20)
project(rein-asset-processing)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MINIZIP REQUIRED IMPORTED_TARGET minizip)
pkg_check_modules(LZ4 REQUIRED IMPORTED_TARGET liblz4)

find_package(Protobuf CONFIG REQUIRED)
find_package(LibLZMA REQUIRED)

enable_language(ISPC)

add_subdirectory(unityasset)

add_executable(process_assets
    Octo/Proto/Database.proto

    astc_dec/astc_decomp.cpp
    astc_dec/astc_decomp.h

    crunch/crn_decomp.h
    crunch/crn_defs.h
    crunch/crnlib.h

    bc7enc_rdo/bc7e.ispc

    asset_reprocessing.cpp
    asset_reprocessing.h
    conversion_context.cpp
    conversion_context.h

    etc2_transcoder.cpp
    etc2_transcoder.h

    HalfFloat.cpp
    HalfFloat.h

    multi_part_zip_writer.cpp
    multi_part_zip_writer.h

    process_assets.cpp
    unzip_handle.cpp
    unzip_handle.h
)
target_link_libraries(process_assets PRIVATE PkgConfig::MINIZIP UnityAsset PkgConfig::LZ4 protobuf::libprotobuf)
target_include_directories(process_assets PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

set_target_properties(process_assets PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED TRUE)
set_source_files_properties(bc7enc_rdo/bc7e.ispc PROPERTIES COMPILE_FLAGS --pic)

protobuf_generate(TARGET process_assets)

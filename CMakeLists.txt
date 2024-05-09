cmake_minimum_required(VERSION 3.20)
project(rein-preservation)

option(BUILD_COMPATIBILITY_RUNTIME "Include the client runtime in the build" ON)
option(BUILD_GAME_SERVER "Include the game server in the build" ON)
option(BUILD_ASSET_PROCESSING "Include the asset processing components in the build" ON)

find_package(Protobuf CONFIG REQUIRED)

if(WIN32 AND NOT TARGET protobuf::protoc)
    message(STATUS "Trying to find the host 'protoc'")

    find_program(HOST_PROTOC protoc REQUIRED)
    add_executable(protobuf::protoc IMPORTED)
    set_target_properties(protobuf::protoc PROPERTIES IMPORTED_LOCATION "${HOST_PROTOC}")
endif()

add_subdirectory(third_party)

add_subdirectory(common)

if(BUILD_COMPATIBILITY_RUNTIME)
    add_subdirectory(CompatibilityRuntime)
endif()

if(BUILD_GAME_SERVER)
    add_subdirectory(GameServer)
    if(NOT CMAKE_CROSSCOMPILING)
        add_subdirectory(reinweb)
    endif()
endif()

if(BUILD_ASSET_PROCESSING)
    add_subdirectory(asset-processing)
endif()

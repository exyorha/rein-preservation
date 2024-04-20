cmake_minimum_required(VERSION 3.20)
project(rein-preservation)

option(BUILD_COMPATIBILITY_RUNTIME "Include the client runtime in the build" ON)
option(BUILD_GAME_SERVER "Include the game server in the build" ON)
option(BUILD_ASSET_PROCESSING "Include the asset processing components in the build" ON)

if(BUILD_COMPATIBILITY_RUNTIME)
    add_subdirectory(CompatibilityRuntime)
endif()

if(BUILD_GAME_SERVER)
    add_subdirectory(GameServer)
endif()

if(BUILD_ASSET_PROCESSING)
    add_subdirectory(asset-processing)
endif()

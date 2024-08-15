cmake_minimum_required(VERSION 3.20)
project(rein-preservation)

function(separate_unix_debug_info target_name)
    if(UNIX OR MINGW)
        add_custom_command(
            TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} --only-keep-debug $<TARGET_FILE:${target_name}> "$<TARGET_FILE_DIR:${target_name}>/$<TARGET_FILE_NAME:${target_name}>.debug"
            COMMAND ${CMAKE_OBJCOPY} --strip-debug --strip-unneeded $<TARGET_FILE:${target_name}>
            COMMAND ${CMAKE_OBJCOPY} --add-gnu-debuglink=$<TARGET_FILE_NAME:${target_name}>.debug $<TARGET_FILE:${target_name}>
            VERBATIM
        )

        install(FILES "$<TARGET_FILE_DIR:${target_name}>/$<TARGET_FILE_NAME:${target_name}>.debug" ${ARGN})
    endif()

endfunction()

option(BUILD_COMPATIBILITY_RUNTIME "Include the client runtime in the build" ON)
option(BUILD_GAME_SERVER "Include the game server in the build" ON)
option(BUILD_ASSET_PROCESSING "Include the asset processing components in the build" ON)

find_package(Protobuf REQUIRED)

if(WIN32 AND NOT TARGET protobuf::protoc)
    message(STATUS "Trying to find the host 'protoc'")

    find_program(HOST_PROTOC protoc REQUIRED)
    add_executable(protobuf::protoc IMPORTED)
    set_target_properties(protobuf::protoc PROPERTIES IMPORTED_LOCATION "${HOST_PROTOC}")
endif()

add_subdirectory(gitversion)

add_subdirectory(third_party)

add_subdirectory(common)

if(BUILD_COMPATIBILITY_RUNTIME)
    add_subdirectory(CompatibilityRuntime)
endif()

if(BUILD_GAME_SERVER)
    if(NOT CMAKE_CROSSCOMPILING)
        add_subdirectory(reinweb)
    endif()
    add_subdirectory(GameServer)
endif()

if(BUILD_ASSET_PROCESSING)
    add_subdirectory(asset-processing)
endif()

cmake_minimum_required(VERSION 3.20)
project(il2cpp-arm-adapter)

include(GenerateExportHeader)

find_program(RUBY ruby REQUIRED)

add_library(GameAssembly MODULE
    il2cpp_api/il2cpp-api-functions.h
    il2cpp_api/il2cpp-api-types.h
    il2cpp_api/il2cpp-api.h
    il2cpp_api_replacements/il2cpp-config-api.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/il2cpp_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/apifuncs.cpp
    ElfModule.cpp
    ElfModule.h
    FileDescriptor.cpp
    FileDescriptor.h
    Image.cpp
    Image.h
    support.h
    support.cpp)
target_include_directories(GameAssembly PRIVATE il2cpp_api_replacements il2cpp_api ${CMAKE_CURRENT_BINARY_DIR}/include .)

install(TARGETS GameAssembly DESTINATION .)
install(FILES app.info DESTINATION LinuxPlayer_Data)

set_target_properties(GameAssembly PROPERTIES
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN TRUE
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)

if(UNIX)
    target_link_options(GameAssembly PRIVATE -Wl,-z,defs "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/version.lds")
    set_target_properties(GameAssembly PROPERTIES LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/version.lds")
endif()

GENERATE_EXPORT_HEADER(
    GameAssembly
    BASE_NAME il2cpp
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/include/il2cpp_export.h
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/apifuncs.cpp
    COMMAND
        ${RUBY} --
        ${CMAKE_CURRENT_SOURCE_DIR}/makeapifuncs.rb
        ${CMAKE_CURRENT_SOURCE_DIR}/il2cpp_api/il2cpp-api-functions.h
        ${CMAKE_CURRENT_BINARY_DIR}/apifuncs.cpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/makeapifuncs.rb
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/il2cpp_api/il2cpp-api-functions.h
    VERBATIM
)

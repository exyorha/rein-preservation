cmake_minimum_required(VERSION 3.20)
project(LogAES)

include(GenerateExportHeader)

add_library(LogAES MODULE
    aes.c
    frontend.c
    frontend.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/aes_export.h
)

set_target_properties(LogAES PROPERTIES
    OUTPUT_NAME FastAES
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN TRUE
)

target_include_directories(LogAES PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/include)

GENERATE_EXPORT_HEADER(
    LogAES
    BASE_NAME LogAES
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/include/aes_export.h
)

install(TARGETS LogAES DESTINATION lib/${ANDROID_ABI})
target_link_libraries(LogAES PRIVATE log)

add_library(gitversion STATIC
    ${CMAKE_CURRENT_BINARY_DIR}/include/gitversion.h
)

set_target_properties(gitversion PROPERTIES LINKER_LANGUAGE C)

target_include_directories(gitversion PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/include)

execute_process(
    COMMAND git describe --always --dirty
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE PROJECT_GITVERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

configure_file(gitversion.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/gitversion.h @ONLY)

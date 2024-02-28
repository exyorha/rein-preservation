include(GenerateExportHeader)

add_library(grpc_csharp_ext SHARED
    grpc_csharp_ext.c
    public/grpc_csharp_ext.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/gpr_export.h
)

target_include_directories(grpc_csharp_ext PUBLIC public ${CMAKE_CURRENT_BINARY_DIR}/include $<TARGET_PROPERTY:gRPC::grpc_unsecure,INTERFACE_INCLUDE_DIRECTORIES>)
target_link_libraries(grpc_csharp_ext PRIVATE gRPC::grpc_unsecure)

set_target_properties(grpc_csharp_ext PROPERTIES
    OUTPUT_NAME "cr_grpc_csharp_ext"
    PREFIX ""
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN TRUE
)


GENERATE_EXPORT_HEADER(
    grpc_csharp_ext
    BASE_NAME GPR
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/include/gpr_export.h
)
install(TARGETS grpc_csharp_ext RUNTIME DESTINATION . COMPONENT GameAssembly)
install(TARGETS grpc_csharp_ext LIBRARY DESTINATION . COMPONENT GameAssembly)

if(MINGW)
    target_link_options(grpc_csharp_ext PRIVATE
        -Wl,-Bstatic -static-libgcc
    )
endif()

add_library(PrecompiledICallThunks MODULE
    ${PRECOMPILED_ICALL_THUNKS}
)
target_link_libraries(PrecompiledICallThunks PRIVATE TranslatorExtensionInterface ZLIB::ZLIB)

set_target_properties(PrecompiledICallThunks PROPERTIES
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN TRUE
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
    PREFIX ""
    LINKER_LANGUAGE C
)

if(UNIX)
    target_link_options(PrecompiledICallThunks PRIVATE
        -Wl,-z,defs
        -Wl,--relax
        -Wl,-z,now
        -Wl,--no-copy-dt-needed
        -static-libstdc++
        -Wl,-z,noexecstack
        -Wl,--hash-style=sysv
        "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/version.lds"
    )
endif()

install(TARGETS PrecompiledICallThunks DESTINATION . COMPONENT GameAssembly)
separate_unix_debug_info(PrecompiledICallThunks DESTINATION . COMPONENT GameAssembly)

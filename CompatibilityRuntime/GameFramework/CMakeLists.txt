include(GenerateExportHeader)

add_library(GameAssembly SHARED
    ${PROJECT_SOURCE_DIR}/thirdparty/il2cpp_api/il2cpp-api-functions.h
    ${PROJECT_SOURCE_DIR}/thirdparty/il2cpp_api/il2cpp-api-types.h
    ${PROJECT_SOURCE_DIR}/thirdparty/il2cpp_api/il2cpp-api.h
    ${PROJECT_SOURCE_DIR}/thirdparty/il2cpp_api/il2cpp-blob.h
    il2cpp_api_replacements/il2cpp-config-api.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/il2cpp_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/apifuncs.cpp
    android_api.cpp
    android_api.h
    translator_main.cpp

    ELF/ElfModule.cpp
    ELF/ElfModule.h
    ELF/Image.cpp
    ELF/Image.h

    FileDescriptor.cpp
    FileDescriptor.h

    GDB/DebugMemoryAccess.cpp
    GDB/DebugMemoryAccess.h
    GDB/GDBPacketFormatter.cpp
    GDB/GDBPacketFormatter.h
    GDB/GDBPacketLayer.cpp
    GDB/GDBPacketLayer.h
    GDB/GDBPacketParser.cpp
    GDB/GDBPacketParser.h
    GDB/GDBStub.cpp
    GDB/GDBStub.h

    GlobalContext.cpp
    GlobalContext.h

    ICall/ARMArgumentPacker.cpp
    ICall/ARMArgumentPacker.h
    ICall/icall_api.cpp
    ICall/ICallInterposerManager.cpp
    ICall/ICallInterposerManager.h
    ICall/InternalCallThunk.cpp
    ICall/InternalCallThunk.h
    ICall/PreparedInternalCall.cpp
    ICall/PreparedInternalCall.h
    ICall/SavedICallContext.cpp
    ICall/SavedICallContext.h

    signal_compatibility.cpp
    signal_compatibility.h
    support.h
    support.cpp

    SystemAPIThunking.cpp
    SystemAPIThunking.h

    Translator/JIT.cpp
    Translator/JIT.h
    Translator/JITThreadContext.cpp
    Translator/JITThreadContext.h
    Translator/thunking.cpp
    Translator/thunking.h
    Translator/ThunkManager.cpp
    Translator/ThunkManager.h
    Translator/thunkutils.S

    translator_api/translator_api.h
)
target_include_directories(GameAssembly PUBLIC il2cpp_api_replacements ${PROJECT_SOURCE_DIR}/thirdparty/il2cpp_api
    ${CMAKE_CURRENT_BINARY_DIR}/include translator_api)
target_include_directories(GameAssembly PRIVATE .)
target_link_libraries(GameAssembly PRIVATE dynarmic PkgConfig::FFI)

set_target_properties(GameAssembly PROPERTIES
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN TRUE
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
    PREFIX ""
)

if(UNIX)
    target_link_options(GameAssembly PRIVATE
        -Wl,-z,defs
        "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/version.lds"
        -Wl,--relax
        -Wl,-Bsymbolic
        -Wl,-z,now
    )
    target_compile_options(GameAssembly PRIVATE
        -ftls-model=initial-exec
    )
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
        ${PROJECT_SOURCE_DIR}/thirdparty/il2cpp_api/il2cpp-api-functions.h
        ${CMAKE_CURRENT_BINARY_DIR}/apifuncs.cpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/makeapifuncs.rb
    DEPENDS ${PROJECT_SOURCE_DIR}/thirdparty/il2cpp_api/il2cpp-api-functions.h
    VERBATIM
)

add_dependencies(GameAssembly armlib)

install(TARGETS GameAssembly DESTINATION . COMPONENT GameAssembly)

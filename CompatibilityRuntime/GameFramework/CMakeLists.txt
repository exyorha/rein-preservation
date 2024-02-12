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

    Bionic/BionicABITypes.h
    Bionic/BionicCallouts.cpp
    Bionic/BionicCallouts.h
    Bionic/BionicThreading.cpp
    Bionic/BionicThreading.h

    ELF/ElfModule.cpp
    ELF/ElfModule.h
    ELF/Image.cpp
    ELF/Image.h

    FileDescriptor.cpp
    FileDescriptor.h

    translator_main.cpp

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

    grpc_channel_redirection.cpp
    grpc_channel_redirection.h
    grpc_csharp_ext.c
    grpc_csharp_ext.h
    grpc_special_thunks.cpp
    grpc_special_thunks.h

    Interop/ARMArgumentPacker.cpp
    Interop/ARMArgumentPacker.h
    Interop/FFIStructureSynthesizer.cpp
    Interop/FFIStructureSynthesizer.h
    Interop/FFISynthesizedStructure.cpp
    Interop/FFISynthesizedStructure.h
    Interop/ICallInterposerManager.cpp
    Interop/ICallInterposerManager.h
    Interop/InternalCallThunk.cpp
    Interop/InternalCallThunk.h
    Interop/InteropCallFrame.cpp
    Interop/InteropCallFrame.h
    Interop/InteropMethodLocator.cpp
    Interop/InteropMethodLocator.h
    Interop/MethodDiversion.cpp
    Interop/MethodDiversion.h
    Interop/PreparedInternalCall.cpp
    Interop/PreparedInternalCall.h
    Interop/SavedICallContext.cpp
    Interop/SavedICallContext.h

    il2cpp-api-internal.h
    il2cpp_api.cpp

    signal_compatibility.cpp
    signal_compatibility.h
    support.h
    support.cpp

    SystemAPIThunking.cpp
    SystemAPIThunking.h

    Translator/BumpAllocator.cpp
    Translator/BumpAllocator.h
    Translator/DiversionManager.cpp
    Translator/DiversionManager.h
    Translator/GCHooks.cpp
    Translator/GCHooks.h
    Translator/JIT.cpp
    Translator/JIT.h
    Translator/JITThreadContext.cpp
    Translator/JITThreadContext.h
    Translator/thunking.cpp
    Translator/thunking.h
    Translator/ThunkManager.cpp
    Translator/ThunkManager.h
    Translator/thunkutils.S
    Translator/WorldStoppingScope.cpp
    Translator/WorldStoppingScope.h

    translator_api/translator_api.h
)
target_include_directories(GameAssembly PUBLIC il2cpp_api_replacements ${PROJECT_SOURCE_DIR}/thirdparty/il2cpp_api
    ${CMAKE_CURRENT_BINARY_DIR}/include translator_api)
target_include_directories(GameAssembly PRIVATE .)
target_link_libraries(GameAssembly PRIVATE dynarmic ffi)

if(NOT WIN32)
    target_link_libraries(GameAssembly PRIVATE gRPC::grpc)
endif()

set_target_properties(GameAssembly PROPERTIES
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN TRUE
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
    PREFIX ""
)

if(FALSE)
set(bdwgc_root ${PROJECT_SOURCE_DIR}/thirdparty/bdwgc)
if(CYGWIN OR WIN32)
    set(bdwgc_sources
        win32_threads.c
    )
else()
    set(bdwgc_sources
        pthread_start.c pthread_stop_world.c pthread_support.c
    )

    if(APPLE)
        list(APPEND bdwgc_sources darwin_stop_world.c)
    endif()


endif()

list(TRANSFORM bdwgc_sources PREPEND "${bdwgc_root}/")
add_library(bdwgc_portion STATIC
    ${bdwgc_sources}
)
target_compile_definitions(bdwgc_portion PRIVATE
    -DALL_INTERIOR_POINTERS -DNO_EXECUTE_PERMISSION -DGC_BUILTIN_ATOMIC -DGC_THREADS -D_REENTRANT -DGC_USESIGRT_SIGNALS
    -DNO_HANDLE_FORK
)
target_include_directories(bdwgc_portion PUBLIC "${bdwgc_root}/include")

if(WIN32)
    target_compile_definitions(bdwgc_portion PRIVATE
        -D_CRT_SECURE_NO_DEPRECATE
    )
endif()
endif()

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

install(TARGETS GameAssembly DESTINATION . COMPONENT GameAssembly)

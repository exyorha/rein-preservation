include(GenerateExportHeader)

# Walk host (x86) thread stacks to mark objects that may be  referenced in
# there. If disabled, GC will instead be disabled during .NET callouts.
set(CR_GARBAGE_COLLECT_HOST_STACKS TRUE)

add_library(GameAssembly SHARED
    ${PROJECT_SOURCE_DIR}/thirdparty/il2cpp_api/il2cpp-api-functions.h
    ${PROJECT_SOURCE_DIR}/thirdparty/il2cpp_api/il2cpp-api-types.h
    ${PROJECT_SOURCE_DIR}/thirdparty/il2cpp_api/il2cpp-api.h
    ${PROJECT_SOURCE_DIR}/thirdparty/il2cpp_api/il2cpp-blob.h
    il2cpp_api_replacements/il2cpp-config-api.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/il2cpp_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/apifuncs.cpp

    Bionic/BionicABITypes.h
    Bionic/BionicAndroidAPI.cpp
    Bionic/BionicAndroidAPI.h
    Bionic/BionicCallouts.cpp
    Bionic/BionicCallouts.h
    Bionic/BionicDynamicLoaderAPI.cpp
    Bionic/BionicDynamicLoaderAPI.h
    Bionic/BionicSyscalls.cpp
    Bionic/BionicSyscalls.h
    Bionic/BionicSyscallTranslation.cpp
    Bionic/BionicThreading.cpp
    Bionic/BionicThreading.h

    ELF/ELFExecutableMapper.h
    ELF/ElfLinkingSet.cpp
    ELF/ElfLinkingSet.h
    ELF/ElfSymbolSource.cpp
    ELF/ElfSymbolSource.h
    ELF/Image.cpp
    ELF/Image.h

    FileDescriptor.cpp
    FileDescriptor.h

    translator_main.cpp

    grpc_special_thunks.cpp
    grpc_special_thunks.h

    GlobalContext.cpp
    GlobalContext.h

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

    MiniGRPC/public/grpc_csharp_ext.h

    MiniGRPC/BatchContext.cpp
    MiniGRPC/BatchContext.h
    MiniGRPC/Call.cpp
    MiniGRPC/Call.h
    MiniGRPC/CallExecutor.cpp
    MiniGRPC/CallExecutor.h
    MiniGRPC/Channel.cpp
    MiniGRPC/Channel.h
    MiniGRPC/ChannelArgs.cpp
    MiniGRPC/ChannelArgs.h
    MiniGRPC/CompletionQueue.cpp
    MiniGRPC/CompletionQueue.h
    MiniGRPC/ExecutingCall.cpp
    MiniGRPC/ExecutingCall.h
    MiniGRPC/GCHandle.cpp
    MiniGRPC/GCHandle.h
    MiniGRPC/grpcsharp.h
    MiniGRPC/grpcsharp_api.cpp
    MiniGRPC/MetadataArray.cpp
    MiniGRPC/MetadataArray.h
    MiniGRPC/SliceBuffer.cpp
    MiniGRPC/SliceBuffer.h
    MiniGRPC/UnityTransport.cpp

    il2cpp-api-internal.h
    il2cpp_api.cpp

    support.h
    support.cpp

    ThunkSymbolSource.cpp
    ThunkSymbolSource.h

    Translator/ARMCallCleanableFrame.cpp
    Translator/ARMCallCleanableFrame.h
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
    Translator/ThreadContextAssociation.h
    Translator/thunking.cpp
    Translator/thunking.h
    Translator/ThunkManager.cpp
    Translator/ThunkManager.h
    Translator/WorldStoppingScope.cpp
    Translator/WorldStoppingScope.h
    Translator/WrappedARMException.cpp
    Translator/WrappedARMException.h

    translator_api/translator_api.h

    GameFramework.rc
)
set_source_files_properties(GameFramework.rc PROPERTIES COMPILE_OPTIONS -c65001)
target_include_directories(GameAssembly PUBLIC il2cpp_api_replacements ${PROJECT_SOURCE_DIR}/thirdparty/il2cpp_api
    ${CMAKE_CURRENT_BINARY_DIR}/include translator_api)
target_include_directories(GameAssembly PRIVATE . ${PROJECT_SOURCE_DIR}/thirdparty/muslelf MiniGRPC/public)
target_link_libraries(GameAssembly PRIVATE dynarmic ffi Il2CppUtilities gitversion)

if(NOT WIN32)
    target_sources(GameAssembly PRIVATE

        Bionic/BionicMmapLinuxHost.cpp
        Bionic/BionicSyscallsLinuxHost.cpp
        Bionic/BionicSyscallTranslationLinuxHost.cpp

        ELF/ElfModule.cpp
        ELF/ElfModule.h
        ELF/ELFExecutableMapperLinux.cpp
        ELF/ELFExecutableMapperLinux.h

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

        Translator/thunkutils.S
        Translator/LinuxTLSProvider.cpp
    )

endif()

if(WIN32)
    target_compile_definitions(GameAssembly PRIVATE -DUNICODE -D_UNICODE -DWIN32_LEAN_AND_MEAN -DNOMINMAX)
    target_sources(GameAssembly PRIVATE
        Bionic/BionicMmapWindowsHost.cpp
        Bionic/BionicSyscallsWindowsHost.cpp
        Bionic/BionicSyscallTranslationWindowsHost.cpp

        ELF/ELFExecutableMapperWindows.cpp
        ELF/ELFExecutableMapperWindows.h

        Translator/WindowsTLSProvider.cpp

        Windows/StringConversions.cpp
        Windows/StringConversions.h
        Windows/WindowsError.cpp
        Windows/WindowsError.h
        Windows/WindowsHandle.cpp
        Windows/WindowsHandle.h
    )
endif()

set_target_properties(GameAssembly PROPERTIES
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN TRUE
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "\$ORIGIN"
    PREFIX ""
)

target_link_libraries(GameAssembly PRIVATE elfpe-interface)

if(CR_GARBAGE_COLLECT_HOST_STACKS)
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
        -DALL_INTERIOR_POINTERS -DNO_EXECUTE_PERMISSION -D_REENTRANT -DGC_USESIGRT_SIGNALS
        -DNO_HANDLE_FORK -DGC_DISABLE_INCREMENTAL -DGC_ALWAYS_MULTITHREADED
        -DGC_NO_THREADS_DISCOVERY
    )
    target_compile_definitions(bdwgc_portion PUBLIC -DGC_THREADS -DGC_BUILTIN_ATOMIC )
    target_include_directories(bdwgc_portion PUBLIC "${bdwgc_root}/include")

    if(WIN32)
        target_compile_definitions(bdwgc_portion PRIVATE
            -D_CRT_SECURE_NO_DEPRECATE -DUSE_SPIN_LOCK
        )
    endif()

    target_link_libraries(GameAssembly PRIVATE bdwgc_portion)

    target_compile_definitions(GameAssembly PRIVATE -DCR_GARBAGE_COLLECT_HOST_STACKS)
endif()

if(UNIX)
    target_link_options(GameAssembly PRIVATE
        -Wl,-z,defs
        "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/version.lds"
        -Wl,--relax
        -Wl,-Bsymbolic
        -Wl,-z,now
        -Wl,--no-copy-dt-needed
        -static-libstdc++
        -Wl,-z,noexecstack
        -Wl,--hash-style=sysv
    )
    target_compile_options(GameAssembly PRIVATE
        -ftls-model=initial-exec
    )
    set_target_properties(GameAssembly PROPERTIES LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/version.lds")
endif()

if(MINGW)
    target_link_options(GameAssembly PRIVATE
        -Wl,-Bstatic -static-libgcc
    )
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

install(TARGETS GameAssembly RUNTIME DESTINATION . COMPONENT GameAssembly)
install(TARGETS GameAssembly LIBRARY DESTINATION . COMPONENT GameAssembly)

include(GenerateExportHeader)

add_library(GameServer SHARED
    public/gameserver_api.h

    ${CMAKE_CURRENT_BINARY_DIR}/include/gameserver_export.h

    Gameserver.cpp
    Gameserver.h
    RPCProcessingThread.cpp
    RPCProcessingThread.h

    main.cpp

    service/DataService.proto
    service/UserService.proto

    ServiceImplementations/UserService.cpp
    ServiceImplementations/UserService.h
)
target_include_directories(GameServer PUBLIC public ${CMAKE_CURRENT_BINARY_DIR}/include)
target_include_directories(GameServer PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

protobuf_generate(TARGET GameServer)
protobuf_generate(
    TARGET GameServer
    LANGUAGE grpc
    PLUGIN protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>
    GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
)

set_target_properties(GameServer PROPERTIES
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN TRUE
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)

target_link_libraries(GameServer PRIVATE gRPC::grpc++ protobuf::libprotobuf)

if(UNIX)
    target_link_options(GameServer PRIVATE
        -Wl,-z,defs
        "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/version.lds"
        -Wl,--relax
        -Wl,-Bsymbolic
        -Wl,-z,now
    )
    target_compile_options(GameServer PRIVATE
        -ftls-model=initial-exec
    )
    set_target_properties(GameServer PROPERTIES LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/version.lds")
endif()

GENERATE_EXPORT_HEADER(
    GameServer
    BASE_NAME GameServer
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/include/gameserver_export.h
)

install(TARGETS GameServer DESTINATION . COMPONENT GameAssembly)

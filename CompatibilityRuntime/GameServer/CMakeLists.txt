include(GenerateExportHeader)

add_library(GameServer SHARED
    public/gameserver_api.h

    ${CMAKE_CURRENT_BINARY_DIR}/include/gameserver_export.h

    DataModel/Database.cpp
    DataModel/Database.h

    DataModel/Sqlite/AggregateFunction.h

    DataModel/Sqlite/Backup.cpp
    DataModel/Sqlite/Backup.h

    DataModel/Sqlite/Blob.cpp
    DataModel/Sqlite/Blob.h

    DataModel/Sqlite/Collation.cpp
    DataModel/Sqlite/Collation.h

    DataModel/Sqlite/Context.h

    DataModel/Sqlite/Database.cpp
    DataModel/Sqlite/Database.h

    DataModel/Sqlite/Error.cpp
    DataModel/Sqlite/Error.h

    DataModel/Sqlite/Function.h

    DataModel/Sqlite/ScalarFunction.h

    DataModel/Sqlite/Statement.cpp
    DataModel/Sqlite/Statement.h

    DataModel/Sqlite/Transaction.cpp
    DataModel/Sqlite/Transaction.h

    DataModel/Sqlite/UpdateHookListener.h

    DataModel/Sqlite/Value.cpp
    DataModel/Sqlite/Value.h

    Gameserver.cpp
    Gameserver.h
    RPCProcessingThread.cpp
    RPCProcessingThread.h

    main.cpp

    service/DataService.proto
    service/GachaService.proto
    service/GamePlayService.proto
    service/GiftService.proto
    service/GimmickService.proto
    service/MissionService.proto
    service/QuestService.proto
    service/UserService.proto

    ServiceImplementations/DataService.cpp
    ServiceImplementations/DataService.h

    ServiceImplementations/GamePlayService.cpp
    ServiceImplementations/GamePlayService.h

    ServiceImplementations/GimmickService.cpp
    ServiceImplementations/GimmickService.h

    ServiceImplementations/QuestService.cpp
    ServiceImplementations/QuestService.h

    ServiceImplementations/UserService.cpp
    ServiceImplementations/UserService.h

    ${CMAKE_CURRENT_BINARY_DIR}/migrations.cpp
)
target_include_directories(GameServer PUBLIC public ${CMAKE_CURRENT_BINARY_DIR}/include)
target_include_directories(GameServer PRIVATE ${CMAKE_CURRENT_BINARY_DIR} .)

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

target_link_libraries(GameServer PRIVATE gRPC::grpc++ protobuf::libprotobuf SQLite::SQLite3 master_database_ext_static)

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

file(GLOB migrations CONFIGURE_DEPENDS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} migrations/*.sql)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/migrations.cpp
    COMMAND ${RUBY} compile_migrations.rb ${CMAKE_CURRENT_BINARY_DIR}/migrations.cpp ${migrations}
    DEPENDS compile_migrations.rb
    DEPENDS ${migrations}
    VERBATIM
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

GENERATE_EXPORT_HEADER(
    GameServer
    BASE_NAME GameServer
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/include/gameserver_export.h
)

install(TARGETS GameServer DESTINATION . COMPONENT GameAssembly)

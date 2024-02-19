cmake_minimum_required(VERSION 3.20)
project(GameServer)

include(GenerateExportHeader)

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LZ4 REQUIRED IMPORTED_TARGET liblz4)
pkg_check_modules(YAJL REQUIRED IMPORTED_TARGET yajl)

find_program(RUBY ruby REQUIRED)

add_library(sqlite3 STATIC
    thirdparty/sqlite3/sqlite3.c
    thirdparty/sqlite3/sqlite3.h
    thirdparty/sqlite3/sqlite3ext.h
)
target_include_directories(sqlite3 PUBLIC thirdparty/sqlite3)
target_compile_definitions(sqlite3 PUBLIC -DSQLITE_ENABLE_SESSION -DSQLITE_ENABLE_PREUPDATE_HOOK)

set_target_properties(sqlite3 PROPERTIES
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN TRUE
)
add_executable(sqlite3-cli thirdparty/sqlite3/shell.c)
target_link_libraries(sqlite3-cli PRIVATE sqlite3)
set_target_properties(sqlite3-cli PROPERTIES OUTPUT_NAME sqlite3)

set(MSGPACK_CXX20 TRUE CACHE BOOL "Using c++20 compiler" FORCE)
set(MSGPACK_USE_BOOST FALSE CACHE BOOL "Use Boost libraried" FORCE)
set(MSGPACK_BUILD_DOCS FALSE CACHE BOOL "Build Doxygen documentation" FORCE)
add_subdirectory(thirdparty/msgpack-c)

add_subdirectory(master_database_ext)

add_executable(GameServer
    DataModel/ChangesetCapturingScope.cpp
    DataModel/ChangesetCapturingScope.h

    DataModel/CurrentNETTimestampFunction.cpp
    DataModel/CurrentNETTimestampFunction.h

    DataModel/Database.cpp
    DataModel/Database.h

    DataModel/DatabaseJSONRepresentation.cpp
    DataModel/DatabaseJSONRepresentation.h

    DataModel/TableChangesetWriter.cpp
    DataModel/TableChangesetWriter.h

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

    DataModel/Sqlite/Session.cpp
    DataModel/Sqlite/Session.h

    DataModel/Sqlite/Statement.cpp
    DataModel/Sqlite/Statement.h

    DataModel/Sqlite/Transaction.cpp
    DataModel/Sqlite/Transaction.h

    DataModel/Sqlite/UpdateHookListener.h

    DataModel/Sqlite/Value.cpp
    DataModel/Sqlite/Value.h

    Gameserver.cpp
    Gameserver.h

    JSONWriter.cpp
    JSONWriter.h

    main.cpp

    service/BannerService.proto
    service/BattleService.proto
    service/CharacterViewerService.proto
    service/DataService.proto
    service/DeckService.proto
    service/DokanService.proto
    service/GachaService.proto
    service/GamePlayService.proto
    service/GiftService.proto
    service/GimmickService.proto
    service/LoginBonusService.proto
    service/MissionService.proto
    service/NaviCutInService.proto
    service/NotificationService.proto
    service/OmikujiService.proto
    service/PortalCageService.proto
    service/QuestService.proto
    service/TutorialService.proto
    service/UserService.proto

    ServiceImplementations/BannerService.cpp
    ServiceImplementations/BannerService.h

    ServiceImplementations/BattleService.cpp
    ServiceImplementations/BattleService.h

    ServiceImplementations/CharacterViewerService.cpp
    ServiceImplementations/CharacterViewerService.h

    ServiceImplementations/CommonService.cpp
    ServiceImplementations/CommonService.h

    ServiceImplementations/DataService.cpp
    ServiceImplementations/DataService.h

    ServiceImplementations/DeckService.cpp
    ServiceImplementations/DeckService.h

    ServiceImplementations/DokanService.cpp
    ServiceImplementations/DokanService.h

    ServiceImplementations/GachaService.cpp
    ServiceImplementations/GachaService.h

    ServiceImplementations/GamePlayService.cpp
    ServiceImplementations/GamePlayService.h

    ServiceImplementations/GimmickService.cpp
    ServiceImplementations/GimmickService.h

    ServiceImplementations/LoginBonusService.cpp
    ServiceImplementations/LoginBonusService.h

    ServiceImplementations/NaviCutInService.cpp
    ServiceImplementations/NaviCutInService.h

    ServiceImplementations/NotificationService.cpp
    ServiceImplementations/NotificationService.h

    ServiceImplementations/OmikujiService.cpp
    ServiceImplementations/OmikujiService.h

    ServiceImplementations/PortalCageService.cpp
    ServiceImplementations/PortalCageService.h

    ServiceImplementations/QuestService.cpp
    ServiceImplementations/QuestService.h

    ServiceImplementations/TutorialService.cpp
    ServiceImplementations/TutorialService.h

    ServiceImplementations/UserService.cpp
    ServiceImplementations/UserService.h

    ${CMAKE_CURRENT_BINARY_DIR}/migrations.cpp
)
target_include_directories(GameServer PRIVATE ${CMAKE_CURRENT_BINARY_DIR} .)

protobuf_generate(TARGET GameServer)
protobuf_generate(
    TARGET GameServer
    LANGUAGE grpc
    PLUGIN protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>
    GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
)

set_target_properties(GameServer PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)

target_link_libraries(GameServer PRIVATE gRPC::grpc++ protobuf::libprotobuf sqlite3 master_database_ext_static
    PkgConfig::YAJL)

file(GLOB migrations CONFIGURE_DEPENDS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} migrations/*.sql)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/migrations.cpp
    COMMAND ${RUBY} compile_migrations.rb ${CMAKE_CURRENT_BINARY_DIR}/migrations.cpp init.sql ${migrations}
    DEPENDS compile_migrations.rb
    DEPENDS ${migrations}
    DEPENDS init.sql
    VERBATIM
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS GameServer DESTINATION . COMPONENT GameServer)


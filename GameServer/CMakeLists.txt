cmake_minimum_required(VERSION 3.20)
project(GameServer)

include(GenerateExportHeader)

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

if(WIN32 AND NOT TARGET protobuf::protoc)
    message(STATUS "Trying to find the host 'protoc'")

    find_program(HOST_PROTOC protoc REQUIRED)
    add_executable(protobuf::protoc IMPORTED)
    set_target_properties(protobuf::protoc PROPERTIES IMPORTED_LOCATION "${HOST_PROTOC}")
endif()

if(WIN32 AND NOT TARGET gRPC::grpc_cpp_plugin)
    message(STATUS "Trying to find the host 'grpc_cpp_plugin'")

    find_program(HOST_GRPC_CPP_PLUGIN grpc_cpp_plugin REQUIRED)
    add_executable(gRPC::grpc_cpp_plugin IMPORTED)
    set_target_properties(gRPC::grpc_cpp_plugin PROPERTIES IMPORTED_LOCATION "${HOST_GRPC_CPP_PLUGIN}")
endif()

add_library(yajl STATIC
    ${CMAKE_CURRENT_BINARY_DIR}/yajl_api_headers/yajl/yajl_common.h
    ${CMAKE_CURRENT_BINARY_DIR}/yajl_api_headers/yajl/yajl_gen.h
    ${CMAKE_CURRENT_BINARY_DIR}/yajl_api_headers/yajl/yajl_parse.h
    ${CMAKE_CURRENT_BINARY_DIR}/yajl_api_headers/yajl/yajl_tree.h
    thirdparty/yajl/src/yajl.c
    thirdparty/yajl/src/yajl_alloc.c
    thirdparty/yajl/src/yajl_alloc.h
    thirdparty/yajl/src/yajl_buf.c
    thirdparty/yajl/src/yajl_buf.h
    thirdparty/yajl/src/yajl_bytestack.h
    thirdparty/yajl/src/yajl_encode.c
    thirdparty/yajl/src/yajl_encode.h
    thirdparty/yajl/src/yajl_gen.c
    thirdparty/yajl/src/yajl_lex.c
    thirdparty/yajl/src/yajl_lex.h
    thirdparty/yajl/src/yajl_parser.c
    thirdparty/yajl/src/yajl_parser.h
    thirdparty/yajl/src/yajl_tree.c
    #thirdparty/yajl/src/yajl_version.c
)
target_include_directories(yajl PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/yajl_api_headers ${CMAKE_CURRENT_BINARY_DIR}/yajl_api_headers/yajl)
target_include_directories(yajl PRIVATE thirdparty/yajl/src)

foreach(yajl_api_header IN ITEMS yajl_common.h yajl_gen.h yajl_parse.h yajl_tree.h)
    target_sources(yajl PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/yajl_api_headers/yajl/${yajl_api_header})
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/yajl_api_headers/yajl/${yajl_api_header}
        COMMAND cmake -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/yajl_api_headers/yajl
        COMMAND cmake -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/yajl/src/api/${yajl_api_header}
            ${CMAKE_CURRENT_BINARY_DIR}/yajl_api_headers/yajl/${yajl_api_header}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/yajl/src/api/${yajl_api_header}
        VERBATIM
    )
endforeach()

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

    DataModel/DatabaseContext.cpp
    DataModel/DatabaseContext.h

    DataModel/DatabaseJSONRepresentation.cpp
    DataModel/DatabaseJSONRepresentation.h

    DataModel/TableChangesetWriter.cpp
    DataModel/TableChangesetWriter.h

    DataModel/UserContext.cpp
    DataModel/UserContext.h

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
    service/CageOrnamentService.proto
    service/CharacterViewerService.proto
    service/CostumeService.proto
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
    service/WeaponService.proto

    ServiceImplementations/BannerService.cpp
    ServiceImplementations/BannerService.h

    ServiceImplementations/BattleService.cpp
    ServiceImplementations/BattleService.h

    ServiceImplementations/CageOrnamentService.cpp
    ServiceImplementations/CageOrnamentService.h

    ServiceImplementations/CharacterViewerService.cpp
    ServiceImplementations/CharacterViewerService.h

    ServiceImplementations/CommonService.cpp
    ServiceImplementations/CommonService.h

    ServiceImplementations/CostumeService.cpp
    ServiceImplementations/CostumeService.h

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

    ServiceImplementations/WeaponService.cpp
    ServiceImplementations/WeaponService.h

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

target_link_libraries(GameServer PRIVATE gRPC::grpc++_unsecure protobuf::libprotobuf sqlite3 master_database_ext_static yajl)

if(MINGW)
    target_link_options(GameServer PRIVATE -Wl,-Bstatic -static-libgcc)
endif()

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


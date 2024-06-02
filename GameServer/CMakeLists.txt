cmake_minimum_required(VERSION 3.20)
project(GameServer)

include(GenerateExportHeader)

find_package(ZLIB REQUIRED)

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

# We bundle a very new version of libevent for the websocket support, so we want static linking.
set(EVENT__LIBRARY_TYPE STATIC CACHE STRING "Set library type to SHARED/STATIC/BOTH (default SHARED for MSVC, otherwise BOTH)" FORCE)

set(EVENT__DISABLE_BENCHMARK ON CACHE BOOL "Defines if libevent should build without the benchmark executables" FORCE)
set(EVENT__DISABLE_TESTS ON CACHE BOOL "If tests should be compiled or not" FORCE)
set(EVENT__DISABLE_REGRESS ON CACHE BOOL "Disable the regress tests" FORCE)
set(EVENT__DISABLE_SAMPLES ON CACHE BOOL "Disable sample files" FORCE)
set(EVENT__DISABLE_OPENSSL ON CACHE STRING "OpenSSL library support: AUTO (use if present), ON (ignore), OFF (require presence)" FORCE)
set(EVENT__DISABLE_MBEDTLS ON CACHE STRING "Mbed TLS library support: AUTO (use if present), ON (ignore), OFF (require presence)" FORCE)


add_subdirectory(thirdparty/libevent)

add_subdirectory(master_database_ext)

add_executable(GameServer
    DatabaseViewer/DatabaseTextImporter.cpp
    DatabaseViewer/DatabaseTextImporter.h
    DatabaseViewer/DatabaseViewerEntity.cpp
    DatabaseViewer/DatabaseViewerEntity.h
    DatabaseViewer/DatabaseViewerEntitySet.cpp
    DatabaseViewer/DatabaseViewerEntitySet.h
    DatabaseViewer/DatabaseViewerResponse.cpp
    DatabaseViewer/DatabaseViewerResponse.h
    DatabaseViewer/DatabaseViewerSchema.cpp
    DatabaseViewer/DatabaseViewerSchema.h
    DatabaseViewer/DatabaseViewerSchemaParser.cpp
    DatabaseViewer/DatabaseViewerSchemaParser.h
    DatabaseViewer/DatabaseViewerService.h
    DatabaseViewer/DatabaseViewerService.cpp
    DatabaseViewer/DatabaseViewerService.h
    DatabaseViewer/DatabaseViewerSQL.cpp
    DatabaseViewer/DatabaseViewerSQL.h
    DatabaseViewer/HierarchicalPathParser.cpp
    DatabaseViewer/HierarchicalPathParser.h
    DatabaseViewer/UTF8Helpers.cpp
    DatabaseViewer/UTF8Helpers.h
    DatabaseViewer/XMLFile.cpp
    DatabaseViewer/XMLFile.h
    DatabaseViewer/TextAssetParser.cpp
    DatabaseViewer/TextXMLWriter.cpp
    DatabaseViewer/TextXMLWriter.h

    DataModel/ChangesetCapturingScope.cpp
    DataModel/ChangesetCapturingScope.h

    DataModel/CurrentNETTimestampFunction.cpp
    DataModel/CurrentNETTimestampFunction.h

    DataModel/Database.cpp
    DataModel/Database.h

    DataModel/DatabaseBackupDeserializer.cpp
    DataModel/DatabaseBackupDeserializer.h

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

    DataModel/Zlib.cpp
    DataModel/Zlib.h

    DataModel/ZlibException.cpp
    DataModel/ZlibException.h

    DataModel/ZlibStream.cpp
    DataModel/ZlibStream.h

    ExitOnEof.cpp
    ExitOnEof.h

    Gameserver.cpp
    Gameserver.h

    GRPC/GRPCLikeCall.cpp
    GRPC/GRPCLikeCall.h
    GRPC/GRPCLikeServer.cpp
    GRPC/GRPCLikeServer.h
    GRPC/PBInputOverLLBuffer.cpp
    GRPC/PBInputOverLLBuffer.h
    GRPC/PBOutputOverLLBuffer.cpp
    GRPC/PBOutputOverLLBuffer.h

    LLServices/JSON/JSONTree.cpp
    LLServices/JSON/JSONTree.h
    LLServices/JSON/JSONWriter.cpp
    LLServices/JSON/JSONWriter.h
    LLServices/JSON/StreamingJSONCallback.cpp
    LLServices/JSON/StreamingJSONCallback.h
    LLServices/JSON/StreamingJSONParser.cpp
    LLServices/JSON/StreamingJSONParser.h
    LLServices/Logging/ConsoleLogSink.cpp
    LLServices/Logging/ConsoleLogSink.h
    LLServices/Logging/LogFacility.cpp
    LLServices/Logging/LogFacility.h
    LLServices/Logging/LogLevel.h
    LLServices/Logging/LogManager.cpp
    LLServices/Logging/LogManager.h
    LLServices/Logging/LogSink.cpp
    LLServices/Logging/LogSink.h
    LLServices/Networking/AfterWaitListener.cpp
    LLServices/Networking/AfterWaitListener.h
    LLServices/Networking/AfterWaitNotifier.cpp
    LLServices/Networking/AfterWaitNotifier.h
    LLServices/Networking/BeforeWaitListener.cpp
    LLServices/Networking/BeforeWaitListener.h
    LLServices/Networking/BeforeWaitNotifier.cpp
    LLServices/Networking/BeforeWaitListener.h
    LLServices/Networking/Buffer.cpp
    LLServices/Networking/Buffer.h
    LLServices/Networking/DeferredDeletable.cpp
    LLServices/Networking/DeferredDeletable.h
    LLServices/Networking/EventLoop.cpp
    LLServices/Networking/EventLoop.h
    LLServices/Networking/EventTypes.h
    LLServices/Networking/FileDescriptor.cpp
    LLServices/Networking/FileDescriptor.h
    LLServices/Networking/FileDescriptorListener.cpp
    LLServices/Networking/FileDescriptorListener.h
    LLServices/Networking/FileDescriptorWatcher.cpp
    LLServices/Networking/FileDescriptorWatcher.h
    LLServices/Networking/HttpRequest.cpp
    LLServices/Networking/HttpRequest.h
    LLServices/Networking/HttpRequestHandler.cpp
    LLServices/Networking/HttpRequestHandler.h
    LLServices/Networking/HttpServer.cpp
    LLServices/Networking/HttpServer.h
    LLServices/Networking/KeyValuePairs.cpp
    LLServices/Networking/KeyValuePairs.h
    LLServices/Networking/PossiblyUnownedPointer.h
    LLServices/Networking/URI.cpp
    LLServices/Networking/URI.h
    LLServices/Networking/URIString.cpp
    LLServices/Networking/URIString.h
    LLServices/Networking/WebSocketConnection.cpp
    LLServices/Networking/WebSocketConnection.h
    LLServices/Networking/WebSocketConnectionListener.cpp
    LLServices/Networking/WebSocketConnectionListener.h
    LLServices/RingBuffer.cpp
    LLServices/RingBuffer.h
    LLServices/ThreadSingleton.h

    main.cpp

    ReloadableResource.cpp
    ReloadableResource.h

    ServerCommandLine.cpp
    ServerCommandLine.h

    WebAPI/WebAPIService.cpp
    WebAPI/WebAPIService.h

    WebAPI/WebDatabase.cpp
    WebAPI/WebDatabase.h

    WebContentServer/LooseFileWebContentStorage.cpp
    WebContentServer/LooseFileWebContentStorage.h

    WebContentServer/WebContentServer.cpp
    WebContentServer/WebContentServer.h

    WebContentServer/WebContentStorage.cpp
    WebContentServer/WebContentStorage.h

    WebContentServer/ZipArchiveWebContentStorage.cpp
    WebContentServer/ZipArchiveWebContentStorage.h

    WebServices/OctoWebServices.cpp
    WebServices/OctoWebServices.h

    WebServices/ServerCLIConnection.cpp
    WebServices/ServerCLIConnection.h

    WebServices/ServerCLIService.cpp
    WebServices/ServerCLIService.h

    WebServices/WebRedirector.cpp
    WebServices/WebRedirector.h

    WebServices/WebRoutable.cpp
    WebServices/WebRoutable.h

    WebServices/WebRouter.cpp
    WebServices/WebRouter.h

    WordListParser.cpp
    WordListParser.h

    service/BannerService.proto
    service/BattleService.proto
    service/CageOrnamentService.proto
    service/CharacterBoardService.proto
    service/CharacterService.proto
    service/CharacterViewerService.proto
    service/ContentsStoryService.proto
    service/CompanionService.proto
    service/CostumeService.proto
    service/DataService.proto
    service/DeckService.proto
    service/DokanService.proto
    service/ExploreService.proto
    service/GachaService.proto
    service/GamePlayService.proto
    service/GiftService.proto
    service/GimmickService.proto
    service/LabyrinthService.proto
    service/LoginBonusService.proto
    service/MissionService.proto
    service/NaviCutInService.proto
    service/NotificationService.proto
    service/OmikujiService.proto
    service/PortalCageService.proto
    service/QuestService.proto
    service/ShopService.proto
    service/SideStoryQuestService.proto
    service/TutorialService.proto
    service/UserService.proto
    service/WeaponService.proto

    ServiceImplementations/BannerService.cpp
    ServiceImplementations/BannerService.h

    ServiceImplementations/BattleService.cpp
    ServiceImplementations/BattleService.h

    ServiceImplementations/CageOrnamentService.cpp
    ServiceImplementations/CageOrnamentService.h

    ServiceImplementations/CharacterBoardService.cpp
    ServiceImplementations/CharacterBoardService.h

    ServiceImplementations/CharacterService.cpp
    ServiceImplementations/CharacterService.h

    ServiceImplementations/CharacterViewerService.cpp
    ServiceImplementations/CharacterViewerService.h

    ServiceImplementations/CommonService.cpp
    ServiceImplementations/CommonService.h

    ServiceImplementations/CompanionService.cpp
    ServiceImplementations/CompanionService.h

    ServiceImplementations/ContentsStoryService.cpp
    ServiceImplementations/ContentsStoryService.cpp

    ServiceImplementations/CostumeService.cpp
    ServiceImplementations/CostumeService.h

    ServiceImplementations/DataService.cpp
    ServiceImplementations/DataService.h

    ServiceImplementations/DeckService.cpp
    ServiceImplementations/DeckService.h

    ServiceImplementations/DokanService.cpp
    ServiceImplementations/DokanService.h

    ServiceImplementations/ExploreService.cpp
    ServiceImplementations/ExploreService.h

    ServiceImplementations/GachaService.cpp
    ServiceImplementations/GachaService.h

    ServiceImplementations/GamePlayService.cpp
    ServiceImplementations/GamePlayService.h

    ServiceImplementations/GiftService.cpp
    ServiceImplementations/GiftService.h

    ServiceImplementations/GimmickService.cpp
    ServiceImplementations/GimmickService.h

    ServiceImplementations/LabyrinthService.cpp
    ServiceImplementations/LabyrinthService.h

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

    ServiceImplementations/ShopService.cpp
    ServiceImplementations/ShopService.h

    ServiceImplementations/SideStoryQuestService.cpp
    ServiceImplementations/SideStoryQuestService.h

    ServiceImplementations/TutorialService.cpp
    ServiceImplementations/TutorialService.h

    ServiceImplementations/UserService.cpp
    ServiceImplementations/UserService.h

    ServiceImplementations/WeaponService.cpp
    ServiceImplementations/WeaponService.h

    ${CMAKE_CURRENT_BINARY_DIR}/migrations.cpp
)
target_include_directories(GameServer PRIVATE ${CMAKE_CURRENT_BINARY_DIR} .)
if(WIN32)
    target_compile_definitions(GameServer PRIVATE -DWIN32_LEAN_AND_MEAN -DNOMINMAX -DUNICODE -D_UNICODE)
    target_link_options(GameServer PRIVATE -municode)

    target_sources(GameServer PRIVATE
        GameServer.rc
    )
    set_source_files_properties(GameServer.rc PROPERTIES COMPILE_OPTIONS -c65001)
endif()

protobuf_generate(TARGET GameServer)

set_target_properties(GameServer PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)

target_link_libraries(GameServer PRIVATE protobuf::libprotobuf sqlite3 master_database_ext_static yajl event ZLIB::ZLIB ClientDataAccess UnityAsset gitversion)
target_include_directories(GameServer PRIVATE ${PROJECT_SOURCE_DIR}/thirdparty/libevent/include
    ${PROJECT_BINARY_DIR}/thirdparty/libevent/include)

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

install(TARGETS GameServer DESTINATION GameServer COMPONENT GameServer)
install(FILES ${PROJECT_SOURCE_DIR}/dbview_schema.sql DESTINATION GameServer COMPONENT GameServer)
install(FILES ${PROJECT_SOURCE_DIR}/dbview_schema.json DESTINATION GameServer COMPONENT GameServer)

if(NOT CMAKE_CROSSCOMPILING)
    add_subdirectory(makewebzip)

    install(
        CODE "execute_process(COMMAND $<TARGET_FILE:makewebzip> \${CMAKE_INSTALL_PREFIX}/GameServer/WebRoot.zip ${GameServer_SOURCE_DIR}/WebRoot ${reinweb_SOURCE_DIR}/public COMMAND_ERROR_IS_FATAL ANY)"
        COMPONENT GameServer)
endif()

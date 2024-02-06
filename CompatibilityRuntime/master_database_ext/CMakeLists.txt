function(master_database_ext_create_target target type)

    add_library(${target} ${type}
        public/master_database_ext.h

        Database.cpp
        Database.h

        DatabaseImage.cpp
        DatabaseImage.h

        DatabaseTable.cpp
        DatabaseTable.h

        extension.cpp

        GlobalContext.cpp
        GlobalContext.h

        MasterDatabaseControlCursor.cpp
        MasterDatabaseControlCursor.h

        MasterDatabaseControlTable.cpp
        MasterDatabaseControlTable.h

        MasterDatabaseCursor.cpp
        MasterDatabaseCursor.h

        MasterDatabaseTable.cpp
        MasterDatabaseTable.h

        ${CMAKE_CURRENT_BINARY_DIR}/${target}_include/master_database_ext_export.h
    )

    if(${type} STREQUAL "STATIC")
        target_compile_definitions(${target} PRIVATE -DSQLITE_CORE)
    endif()

    target_include_directories(${target} PUBLIC public ${CMAKE_CURRENT_BINARY_DIR}/${target}_include)
    target_include_directories(${target} PRIVATE ${SQLite3_INCLUDE_DIRS})

    if(UNIX AND ${type} STREQUAL "SHARED")
        target_link_options(${target} PRIVATE -Wl,-z,defs,-z,now)
    endif()

    target_link_libraries(${target} PRIVATE  OpenSSL::Crypto msgpack-cxx PkgConfig::LZ4)

    set_target_properties(${target} PROPERTIES
        C_VISIBILITY_PRESET hidden
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN TRUE
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED TRUE
        POSITION_INDEPENDENT_CODE TRUE
    )


    GENERATE_EXPORT_HEADER(
        ${target}
        BASE_NAME master_database_ext
        EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/${target}_include/master_database_ext_export.h
    )

endfunction()

include(GenerateExportHeader)

master_database_ext_create_target(master_database_ext SHARED)
master_database_ext_create_target(master_database_ext_static STATIC)


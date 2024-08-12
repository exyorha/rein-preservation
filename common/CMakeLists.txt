add_subdirectory(ClientDataAccess)

add_subdirectory(ConfigFile)

if(BUILD_GAME_SERVER OR BUILD_ASSET_PROCESSING)
    add_subdirectory(unityasset)
    add_subdirectory(ZipFileWriter)
endif()

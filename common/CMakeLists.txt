add_subdirectory(ClientDataAccess)

if(BUILD_GAME_SERVER OR BUILD_ASSET_PROCESSING)
    add_subdirectory(unityasset)
endif()

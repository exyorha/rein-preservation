add_subdirectory(ClientDataAccess)

add_subdirectory(ConfigFile)

if(BUILD_GAME_SERVER OR BUILD_ASSET_PROCESSING)
    add_subdirectory(unityasset)
    unity_content_generate_library(UnityAsset U2019.4.9f1)
    add_subdirectory(ZipFileWriter)
endif()

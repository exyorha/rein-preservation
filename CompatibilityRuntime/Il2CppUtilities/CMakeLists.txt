add_library(Il2CppUtilities INTERFACE)

target_sources(Il2CppUtilities INTERFACE

    public/Il2CppUtilities.h
    Il2CppUtilities.cpp
)

target_include_directories(Il2CppUtilities INTERFACE
    public
    ${PROJECT_SOURCE_DIR}/thirdparty/il2cpp_api
    ${PROJECT_SOURCE_DIR}/GameFramework/il2cpp_api_replacements)

if(WIN32)

    target_sources(Il2CppUtilities INTERFACE
        Il2CppUtilitiesWindows.cpp
    )
else()

    target_sources(Il2CppUtilities INTERFACE
        Il2CppUtilitiesIconv.cpp
    )

    target_link_libraries(Il2CppUtilities INTERFACE Iconv::Iconv)

endif()


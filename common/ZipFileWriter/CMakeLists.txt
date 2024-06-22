find_package(ZLIB REQUIRED)

add_library(ZipFileWriter STATIC
    include/ZipFileWriter/MultiPartZipWriter.h
    MultiPartZipWriter.cpp
)

target_include_directories(ZipFileWriter PUBLIC include)

set_target_properties(ZipFileWriter PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
    POSITION_INDEPENDENT_CODE TRUE
)

target_link_libraries(ZipFileWriter PUBLIC UnityAsset)
target_link_libraries(ZipFileWriter PRIVATE ZLIB::ZLIB)

add_executable(makewebzip main.cpp)
set_target_properties(makewebzip PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)
target_link_libraries(makewebzip PRIVATE ZipFileWriter)

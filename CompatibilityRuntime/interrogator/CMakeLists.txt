foreach(executable IN ITEMS interrogator extract-descriptors)
    add_executable(${executable} EXCLUDE_FROM_ALL ${executable}.cpp)
    target_link_libraries(${executable} PRIVATE GameAssembly)
    target_link_options(${executable} PRIVATE -Wl,-rpath,$ORIGIN)
endforeach()

target_link_libraries(extract-descriptors PRIVATE protobuf::libprotobuf)


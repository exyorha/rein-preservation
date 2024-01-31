foreach(executable IN ITEMS interrogator extract-descriptors)
    add_executable(${executable} ${executable}.cpp)
    target_link_libraries(${executable} PRIVATE GameAssembly)
    target_link_options(${executable} PRIVATE -Wl,-rpath,$ORIGIN)

    install(TARGETS ${executable} DESTINATION . COMPONENT GameAssembly)
endforeach()

pkg_check_modules(ProtocolBuffers REQUIRED IMPORTED_TARGET protobuf)

target_link_libraries(extract-descriptors PRIVATE PkgConfig::ProtocolBuffers)


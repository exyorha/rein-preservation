add_executable(interrogator main.cpp)
target_link_libraries(interrogator PRIVATE GameAssembly)

target_link_options(interrogator PRIVATE -Wl,-rpath,$ORIGIN)

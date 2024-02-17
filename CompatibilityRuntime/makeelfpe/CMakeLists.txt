add_library(elfpe-interface INTERFACE)
target_include_directories(elfpe-interface INTERFACE elfpe_interface)

if(NOT CMAKE_CROSSCOMPILING)

    add_executable(makeelfpe
        main.cpp
    )

    target_include_directories(makeelfpe PRIVATE ${PROJECT_SOURCE_DIR}/thirdparty/muslelf)
    target_link_libraries(makeelfpe PRIVATE elfpe-interface)
endif()

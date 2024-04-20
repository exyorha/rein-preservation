add_library(ClientDataAccess STATIC
    Octo/Proto/Database.proto

    include/ClientDataAccess/OctoContentStorage.h
    OctoContentStorage.cpp

    include/ClientDataAccess/ZIPBasedStorage.h
    ZIPBasedStorage.cpp
)

protobuf_generate(TARGET ClientDataAccess)

target_link_libraries(ClientDataAccess PUBLIC protobuf::libprotobuf)

set_target_properties(ClientDataAccess PROPERTIES
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN TRUE
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
    POSITION_INDEPENDENT_CODE TRUE
)
target_include_directories(ClientDataAccess PUBLIC ${CMAKE_CURRENT_BINARY_DIR} include)

add_library(UnityStub SHARED
    unity_stub.cpp
    unity_stub.h
)

target_include_directories(UnityStub PUBLIC .)
set_target_properties(UnityStub PROPERTIES
    OUTPUT_NAME UnityPlayer
    PREFIX ""
)

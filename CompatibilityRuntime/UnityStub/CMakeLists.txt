add_library(UnityStub SHARED
    unity_stub.cpp
    include/unity_stub.h
)

target_include_directories(UnityStub PUBLIC include)
target_compile_definitions(UnityStub PRIVATE -DUNITY_STUB_BUILD)
set_target_properties(UnityStub PROPERTIES
    OUTPUT_NAME UnityPlayer
    PREFIX ""
)

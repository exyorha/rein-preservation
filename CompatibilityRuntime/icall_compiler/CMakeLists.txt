add_executable(icall_compiler
    ArgumentPackage.h
    ICallMethod.cpp
    ICallMethod.h
    ICallMethodRegistry.cpp
    ICallMethodRegistry.h
    ICallType.cpp
    ICallType.h
    main.cpp
)

target_link_libraries(icall_compiler PRIVATE GameAssembly ZLIB::ZLIB)

set_target_properties(icall_compiler PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)

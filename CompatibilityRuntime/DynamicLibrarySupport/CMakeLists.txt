add_library(PlatformDynamicLibrary STATIC
    include/PlatformDynamicLibrary.h
)
target_include_directories(PlatformDynamicLibrary PUBLIC include)

if(NOT WIN32)
    target_sources(PlatformDynamicLibrary PRIVATE
        LinuxDynamicLibrary.cpp
        include/LinuxDynamicLibrary.h
    )
else()
    target_sources(PlatformDynamicLibrary PRIVATE
        WindowsDynamicLibrary.cpp
        include/WindowsDynamicLibrary.h
    )
endif()

target_include_directories(PlatformDynamicLibrary PRIVATE include)
target_compile_definitions(PlatformDynamicLibrary PRIVATE -DWIN32_LEAN_AND_MEAN -DUNICODE -D_UNICODE -DNOMINMAX)
set_target_properties(PlatformDynamicLibrary PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)

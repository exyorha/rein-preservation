add_library(ConfigFile STATIC
    ConfigFileParser.cpp
    include/ConfigFileParser.h
)

set_target_properties(ConfigFile PROPERTIES
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN TRUE
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
    POSITION_INDEPENDENT_CODE TRUE
)
target_include_directories(ConfigFile PUBLIC include)

if(BUILD_GAME_SERVER OR BUILD_ASSET_PROCESSING)
    add_library(lz4 STATIC
        lz4/lib/lz4.c
        lz4/lib/lz4.h
        lz4/lib/lz4hc.c
        lz4/lib/lz4hc.h
    )
    target_include_directories(lz4 PUBLIC lz4/lib)

    set_target_properties(lz4 PROPERTIES
        C_VISIBILITY_PRESET hidden
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN TRUE
        POSITION_INDEPENDENT_CODE TRUE
    )
endif()

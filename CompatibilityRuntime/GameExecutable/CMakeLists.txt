add_executable(GameExecutable main.cpp)
target_link_libraries(GameExecutable PRIVATE UnityStub GameAssembly)
set_target_properties(GameExecutable PROPERTIES
    OUTPUT_NAME NieR
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "\$ORIGIN"
    LINKER_LANGUAGE C
)

if(UNIX)
    target_link_options(GameExecutable PRIVATE -Wl,-z,now)
endif()

install(TARGETS GameExecutable DESTINATION . COMPONENT GameAssembly)
install(FILES app.info DESTINATION $<TARGET_FILE_BASE_NAME:GameExecutable>_Data COMPONENT GameAssembly)

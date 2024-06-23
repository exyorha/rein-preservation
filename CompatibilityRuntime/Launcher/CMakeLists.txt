include(CheckIPOSupported)

add_executable(Launcher WIN32
    LauncherApplication.cpp
    LauncherApplication.h

    LauncherApplicationInterface.cpp
    LauncherApplicationInterface.h

    LauncherConfiguration.cpp
    LauncherConfiguration.h

    LauncherPlatform.cpp
    LauncherPlatform.h

    main.cpp
    nuklear.h
    nuklear_config.cpp
    nuklear_config.h
    nuklear_xlib.h
)

if(WIN32)
    target_sources(Launcher PRIVATE
        LauncherPlatformWin32.cpp
        LauncherPlatformWin32.h

        Launcher.rc
    )

    target_compile_definitions(Launcher PRIVATE -DWIN32_LEAN_AND_MEAN -DUNICODE -D_UNICODE -DNOMINMAX)
    target_link_options(Launcher PRIVATE -Wl,-Bstatic -static-libgcc)
    target_link_libraries(Launcher PRIVATE msimg32 shell32)
    set_source_files_properties(Launcher.rc PROPERTIES COMPILE_OPTIONS -c65001)
else()
    target_sources(Launcher PRIVATE
        LauncherPlatformX11.cpp
        LauncherPlatformX11.h
    )

    find_package(X11 REQUIRED)
    target_link_libraries(Launcher PRIVATE X11::X11 X11::Xft X11::Xrandr)
    target_link_options(Launcher PRIVATE -static-libstdc++ -static-libgcc -Wl,--hash-style=sysv)
endif()

set_target_properties(Launcher PROPERTIES
    OUTPUT_NAME NieRLauncher
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)
target_link_libraries(Launcher PRIVATE gitversion)

check_ipo_supported(RESULT supported OUTPUT error)
if(supported)
    set_target_properties(Launcher PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

if(NOT MSVC)
    target_link_options(Launcher PRIVATE -Wl,--gc-sections)
endif()

if(MINGW)
    target_link_options(Launcher PRIVATE -municode)
endif()

install(TARGETS Launcher DESTINATION . COMPONENT GameAssembly)

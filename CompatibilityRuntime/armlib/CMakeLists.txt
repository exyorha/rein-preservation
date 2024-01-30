add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/armlib.so
    COMMAND
        aarch64-linux-gnu-gcc
        -shared
        -o ${CMAKE_CURRENT_BINARY_DIR}/armlib.so
        ${CMAKE_CURRENT_SOURCE_DIR}/armlib.c
        -x assembler-with-cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/setjmp.S
        -nostdinc
        -nostartfiles
        -ffreestanding
        -Wall -W
        -O2
        -fPIC
        -Wl,-Bsymbolic
        -Wl,--hash-style=sysv
        -Wl,-z,norelro
        -lgcc
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/armlib.c
        ${CMAKE_CURRENT_SOURCE_DIR}/setjmp.S
        ${CMAKE_CURRENT_SOURCE_DIR}/nanoprintf.h
)

add_custom_target(armlib DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/armlib.so)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/armlib.so DESTINATION . COMPONENT GameAssembly)

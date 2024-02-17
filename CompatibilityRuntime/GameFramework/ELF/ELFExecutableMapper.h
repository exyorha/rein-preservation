#ifndef ELF_ELF_EXECUTABLE_MAPPER_H
#define ELF_ELF_EXECUTABLE_MAPPER_H

#ifdef _WIN32

#include <ELF/ELFExecutableMapperWindows.h>

using ELFExecutableMapper = ELFExecutableMapperWindows;

#else

#include <ELF/ELFExecutableMapperLinux.h>

using ELFExecutableMapper = ELFExecutableMapperLinux;

#endif

#endif


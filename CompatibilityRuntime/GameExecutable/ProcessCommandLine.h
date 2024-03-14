#ifndef PROCESS_COMMAND_LINE_H
#define PROCESS_COMMAND_LINE_H

#ifdef _WIN32
#include "ProcessCommandLineWindows.h"

using ProcessCommandLine = ProcessCommandLineWindows;
#else
#include "ProcessCommandLineLinux.h"

using ProcessCommandLine = ProcessCommandLineLinux;
#endif

#endif

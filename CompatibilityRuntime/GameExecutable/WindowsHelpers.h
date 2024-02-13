#ifndef WINDOWS_HELPERS_H
#define WINDOWS_HELPERS_H

#include <windows.h>

#include <filesystem>

std::filesystem::path getPathToModule(HMODULE moduleHandle);

#endif

#ifndef SUPPORT_H
#define SUPPORT_H

#include <filesystem>

[[noreturn]] void stubcall(const char *format, ...);

std::filesystem::path thisLibraryDirectory();

#endif

#ifndef SUPPORT_H
#define SUPPORT_H

#include <filesystem>

[[noreturn]] void panic(const char *format, ...);

std::filesystem::path thisLibraryDirectory();

#endif

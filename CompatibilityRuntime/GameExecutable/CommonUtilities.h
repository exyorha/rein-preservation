#ifndef COMMON_UTILITIES_H
#define COMMON_UTILITIES_H

#include <filesystem>

bool stringsEqualIgnoreCaseAsciiOnly(const char *a, const char *b);

std::filesystem::path executablePath();
std::filesystem::path executableDirectory();
std::filesystem::path getApplicationDataDir();

#endif

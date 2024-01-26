#ifndef IL2CPP_UTILITIES_H
#define IL2CPP_UTILITIES_H

#include <il2cpp-api.h>
#include <string>
#include <string_view>

std::string stringToUtf8(Il2CppString *url);
Il2CppString *stringFromUtf8(const std::string_view &conv);

std::u16string_view stringToUtf16(Il2CppString *url);

std::string utf16ToUtf8(const std::u16string_view &utf16);

#endif

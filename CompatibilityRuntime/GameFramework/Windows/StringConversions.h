#ifndef STRING_CONVERSIONS__H
#define STRING_CONVERSIONS__H

#include <string>

std::wstring utf8ToWide(const std::string_view& string);

std::string wideToUtf8(const std::wstring_view& string);

std::wstring codepageToWide(const std::string_view& string, unsigned int codepage);

std::string wideToCodepage(const std::wstring_view& string, unsigned int codepage);

#endif

#include "Il2CppUtilities.h"

#include <windows.h>

#include <stdexcept>

template<typename T>
static std::string wideToCodepage(const std::basic_string_view<T>& string, unsigned int codepage) {
    static_assert(sizeof(T) == 2, "wideToCodepage must be called on a wide string");

    if (string.empty())
        return std::string();

    std::string output;
    output.resize(string.size() * 4); // Worst case: every UTF-16 wchar_t generates 4 UTF-8 bytes (ignoring invalid 5, 6 byte UTF-8 sequences)

    int characters = WideCharToMultiByte(codepage, 0, reinterpret_cast<const wchar_t *>(string.data()),
                                         (int)string.size(), &output[0], (int)output.size(), nullptr, nullptr);
    if (characters == 0)
        throw std::runtime_error("WideCharToMultiByte has failed");

    output.resize(characters);

    return output;
}

std::string utf16ToUtf8(const std::u16string_view &utf16) {
    return wideToCodepage(utf16, CP_UTF8);
}

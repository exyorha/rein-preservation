#include "Il2CppUtilities.h"

std::u16string_view stringToUtf16(Il2CppString *url) {
    auto length = il2cpp_string_length(url);
    auto chars = reinterpret_cast<const char16_t *>(il2cpp_string_chars(url));

    return std::u16string_view(chars, length);
}

std::string stringToUtf8(Il2CppString *url) {
    return utf16ToUtf8(stringToUtf16(url));
}

Il2CppString *stringFromUtf8(const std::string_view &conv) {
    return il2cpp_string_new_len(conv.data(), conv.size());
}

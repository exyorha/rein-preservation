#include "Il2CppUtilities.h"

uint32_t Il2CppVMCharacteristics::objectHeaderSize;
uint32_t Il2CppVMCharacteristics::arrayObjectHeaderSize;
uint32_t Il2CppVMCharacteristics::offsetOfArrayLengthInArrayObjectHeader;
uint32_t Il2CppVMCharacteristics::offsetOfArrayBoundsInArrayObjectHeader;
uint32_t Il2CppVMCharacteristics::allocationGranularity;

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

Il2CppString *stringFromUtf16(const std::u16string_view &conv) {
    return il2cpp_string_new_utf16(reinterpret_cast<const Il2CppChar*>(conv.data()), conv.size());
}

void Il2CppVMCharacteristics::initialize() {
    objectHeaderSize = il2cpp_object_header_size();
    arrayObjectHeaderSize = il2cpp_array_object_header_size();
    offsetOfArrayLengthInArrayObjectHeader = il2cpp_offset_of_array_length_in_array_object_header();
    offsetOfArrayBoundsInArrayObjectHeader = il2cpp_offset_of_array_bounds_in_array_object_header();
    allocationGranularity = il2cpp_allocation_granularity();
}

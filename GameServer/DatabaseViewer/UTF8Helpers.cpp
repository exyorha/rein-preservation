#include "DatabaseViewer/UTF8Helpers.h"

std::optional<int> classifyUTF8Byte(char ch) {
    auto uch = static_cast<unsigned char>(ch);

    // continuation byte
    if((uch & 0xC0) == 0x80)
        return -1;

    if((uch & 0x80) == 0x00) {
        // ASCII-compatible character.

        return 0;
    } else if((uch & 0xE0) == 0xC0) {

        return 1;
    } else if((uch & 0xF0) == 0xE0) {

        return 2;
    } else if((uch & 0xF8) == 0xF0) {

        return 3;
    } else {
        return {};
    }
}

#ifndef DATA_MODEL_ZLIB_H
#define DATA_MODEL_ZLIB_H

#include <string_view>
#include <string>

void inflate(const unsigned char *source, size_t sourceLength, unsigned char *destination, size_t destinationLength);

std::string inflate(const std::string_view &source, bool gzip = false);

std::string deflate(const std::string_view &source, bool gzip = false);

#endif


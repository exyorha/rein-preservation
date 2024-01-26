#ifndef DEBUG_MEMORY_ACCESS_H
#define DEBUG_MEMORY_ACCESS_H

#include <cstring>
#include <cstdint>

bool debugMemoryRead(uintptr_t address, void *dest, size_t length);

bool debugMemoryWrite(uintptr_t address, const void *dest, size_t length);

#endif

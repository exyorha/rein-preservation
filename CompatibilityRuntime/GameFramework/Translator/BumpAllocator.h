#ifndef TRANSLATOR_BUMP_ALLOCATOR_H
#define TRANSLATOR_BUMP_ALLOCATOR_H

#include <cstring>

class BumpAllocator {
public:
    BumpAllocator();
    ~BumpAllocator();

    BumpAllocator(const BumpAllocator &other) = delete;
    BumpAllocator &operator =(const BumpAllocator &other) = delete;

    void *allocate(size_t size);

private:
    static constexpr size_t ThunkBlockAllocationSize = 65536;
    unsigned char *m_currentBlock;
    unsigned char *m_currentBlockEnd;
};

#endif

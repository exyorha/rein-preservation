#include <Translator/BumpAllocator.h>

#ifdef _WIN32
#include <windows.h>
#include <Windows/WindowsError.h>
#else
#include <sys/mman.h>
#endif

#include <system_error>

BumpAllocator::BumpAllocator() : m_currentBlock(nullptr), m_currentBlockEnd(nullptr) {

}

BumpAllocator::~BumpAllocator() = default;

void *BumpAllocator::allocate(size_t size) {
    if(m_currentBlockEnd - m_currentBlock < size) {
#ifdef _WIN32
        m_currentBlock = static_cast<unsigned char *>(
            VirtualAlloc(nullptr, ThunkBlockAllocationSize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE));

        if(!m_currentBlock)
            WindowsError::throwLastError();
#else
        m_currentBlock = static_cast<unsigned char *>(
            mmap(nullptr, ThunkBlockAllocationSize, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)
        );
        if(m_currentBlock == MAP_FAILED)
            throw std::system_error(errno, std::generic_category());
#endif

        m_currentBlockEnd = m_currentBlock + ThunkBlockAllocationSize;
    }

    auto ptr = m_currentBlock;
    m_currentBlock += size;

    return ptr;
}


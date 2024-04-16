#ifndef IL2CPP_UTILITIES_H
#define IL2CPP_UTILITIES_H

#include <il2cpp-api.h>
#include <string>
#include <string_view>

std::string stringToUtf8(Il2CppString *url);
Il2CppString *stringFromUtf8(const std::string_view &conv);

std::u16string_view stringToUtf16(Il2CppString *url);
Il2CppString *stringFromUtf16(const std::u16string_view &conv);

std::string utf16ToUtf8(const std::u16string_view &utf16);
std::u16string utf8ToUtf16(const std::string_view &utf8);

class Il2CppVMCharacteristics {
public:
    static uint32_t objectHeaderSize;
    static uint32_t arrayObjectHeaderSize;
    static uint32_t offsetOfArrayLengthInArrayObjectHeader;
    static uint32_t offsetOfArrayBoundsInArrayObjectHeader;
    static uint32_t allocationGranularity;

    static void initialize();
};

template<typename T>
class ArrayWrapper {
public:
    inline explicit ArrayWrapper(Il2CppArray *array) : m_array(array) {

    }

    ~ArrayWrapper() = default;

    ArrayWrapper(const ArrayWrapper &other) = delete;
    ArrayWrapper &operator =(const ArrayWrapper &other) = delete;

    ArrayWrapper(ArrayWrapper &&other) noexcept = default;
    ArrayWrapper &operator =(ArrayWrapper &&other) noexcept = default;

    inline size_t size() const {
        return *sizePtr();
    }

    inline T *data() const {
        return reinterpret_cast<T *>(reinterpret_cast<unsigned char *>(m_array) + Il2CppVMCharacteristics::arrayObjectHeaderSize);
    }

    inline T &operator[](size_t index) const {
        return data()[index];
    }

    inline Il2CppArray *array() const {
        return m_array;
    }

    inline Il2CppObject *object() const {
        return reinterpret_cast<Il2CppObject *>(m_array);
    }

private:

    inline uint32_t *sizePtr() const {
        return reinterpret_cast<uint32_t *>(reinterpret_cast<unsigned char *>(m_array) + Il2CppVMCharacteristics::offsetOfArrayLengthInArrayObjectHeader);
    }

    inline uint32_t *boundsPtr() const {
        return reinterpret_cast<uint32_t *>(reinterpret_cast<unsigned char *>(m_array) + Il2CppVMCharacteristics::offsetOfArrayBoundsInArrayObjectHeader);
    }

private:
    Il2CppArray *m_array;
};

#endif

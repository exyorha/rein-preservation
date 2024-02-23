#ifndef JAVA_JNI_BASE_PRIMITIVE_ARRAY_H
#define JAVA_JNI_BASE_PRIMITIVE_ARRAY_H

#include <vector>

template<typename T>
class JNIBasePrimitiveArray {
public:
    explicit JNIBasePrimitiveArray(size_t size) : m_storage(size) {

    }

    ~JNIBasePrimitiveArray() = default;

    JNIBasePrimitiveArray(const JNIBasePrimitiveArray &other) = delete;
    JNIBasePrimitiveArray &operator =(const JNIBasePrimitiveArray &other) = delete;

    inline const std::vector<T> &data() const {
        return m_storage;
    }

    inline std::vector<T> &data() {
        return m_storage;
    }

private:
    std::vector<T> m_storage;
};

#endif

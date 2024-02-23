#ifndef JAVA_JNI_FLOAT_ARRAY_H
#define JAVA_JNI_FLOAT_ARRAY_H

#include <Java/JNIObject.h>
#include <Java/JNIBasePrimitiveArray.h>

class JNIFloatArray final : public JNIObject, public JNIBasePrimitiveArray<float> {
public:

    explicit JNIFloatArray(size_t size);
    ~JNIFloatArray();

    static std::shared_ptr<JNIClass> makeClass();
};

#endif


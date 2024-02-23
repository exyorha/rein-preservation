#include <Java/JNIFloatArray.h>
#include <Java/JNIClass.h>

JNIFloatArray::JNIFloatArray(size_t size) :
    JNIObject(parent("[F")),
    JNIBasePrimitiveArray<float>(size) {

}

JNIFloatArray::~JNIFloatArray() = default;


std::shared_ptr<JNIClass> JNIFloatArray::makeClass() {
    auto co = std::make_shared<JNIClass>("[F", parent("java/lang/Object"));

    return co;
}

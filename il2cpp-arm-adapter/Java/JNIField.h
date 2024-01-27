#ifndef JAVA_JNI_FIELD_H
#define JAVA_JNI_FIELD_H

#include <Java/JNIAccessibleObject.h>


class JNIField final : public JNIObject {
public:
    static std::shared_ptr<JNIClass> makeClass();

    std::shared_ptr<JNIObject> getDeclaringClass(Il2CppArray *args);
};

#endif

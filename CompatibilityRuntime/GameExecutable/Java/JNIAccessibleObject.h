#ifndef JAVA_JNI_ACCESSIBLE_OBJECT_H
#define JAVA_JNI_ACCESSIBLE_OBJECT_H

#include <Java/JNIObject.h>

class JNIAccessibleObject : public JNIObject {
public:
    explicit JNIAccessibleObject(const std::shared_ptr<JNIClass> &objectClass);
    ~JNIAccessibleObject() override;

    static std::shared_ptr<JNIClass> makeClass();
};


#endif

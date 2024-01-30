#ifndef JAVA_JNI_ACCESSIBLE_OBJECT_H
#define JAVA_JNI_ACCESSIBLE_OBJECT_H

#include <Java/JNIObject.h>

class JNIAccessibleObject final : public JNIObject {
public:
    static std::shared_ptr<JNIClass> makeClass();
};


#endif

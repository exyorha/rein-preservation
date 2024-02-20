#ifndef JAVA_JNI_EXECUTABLE_H
#define JAVA_JNI_EXECUTABLE_H

#include <Java/JNIAccessibleObject.h>
#include <Java/JNIClass.h>

class JNIExecutable : public JNIAccessibleObject {
public:
    explicit JNIExecutable(const std::shared_ptr<JNIClass> &objectClass);
    ~JNIExecutable() override;

    static std::shared_ptr<JNIClass> makeClass();

    virtual const JNIClass::RegisteredMethod *method() const = 0;
};

#endif


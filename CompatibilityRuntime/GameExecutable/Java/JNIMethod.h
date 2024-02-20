#ifndef JAVA_JNI_METHOD_H
#define JAVA_JNI_METHOD_H

#include <Java/JNIExecutable.h>

#include <Java/JNIClass.h>

class JNIMethod final : public JNIExecutable {
public:
    explicit JNIMethod(const std::shared_ptr<JNIClass> &declaringClass, const JNIClass::RegisteredMethod &method);
    ~JNIMethod() override;

    static std::shared_ptr<JNIClass> makeClass();

    const JNIClass::RegisteredMethod *method() const override;

private:
    std::shared_ptr<JNIClass> m_declaringClass;
    const JNIClass::RegisteredMethod &m_method;
};

#endif


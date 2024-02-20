#ifndef JAVA_JNI_CONSTRUCTOR_H
#define JAVA_JNI_CONSTRUCTOR_H

#include <Java/JNIExecutable.h>

#include <Java/JNIClass.h>

class JNIConstructor final : public JNIExecutable {
public:
    explicit JNIConstructor(const std::shared_ptr<JNIClass> &declaringClass,
                            const JNIClass::RegisteredMethod &method);
    ~JNIConstructor() override;

    static std::shared_ptr<JNIClass> makeClass();

    const JNIClass::RegisteredMethod *method() const override;

private:
    std::shared_ptr<JNIClass> m_declaringClass;
    const JNIClass::RegisteredMethod &m_method;
};

#endif



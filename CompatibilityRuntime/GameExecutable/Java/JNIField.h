#ifndef JAVA_JNI_FIELD_H
#define JAVA_JNI_FIELD_H

#include <Java/JNIAccessibleObject.h>
#include <Java/JNIClass.h>

class JNIField final : public JNIAccessibleObject {
public:
    explicit JNIField(const std::shared_ptr<JNIClass> &declaringClass, const JNIClass::RegisteredField &field);
    ~JNIField() override;

    static std::shared_ptr<JNIClass> makeClass();

    std::shared_ptr<JNIObject> getDeclaringClass();

    inline const JNIClass::RegisteredField *field() const {
        return &m_field;
    }

private:
    std::shared_ptr<JNIClass> m_declaringClass;
    const JNIClass::RegisteredField &m_field;
};

#endif

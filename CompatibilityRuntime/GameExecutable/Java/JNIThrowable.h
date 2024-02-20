#ifndef JAVA_JNI_THROWABLE_H
#define JAVA_JNI_THROWABLE_H

#include <Java/JNIObject.h>

class JNIThrowable final : public JNIObject {
public:
    JNIThrowable();
    explicit JNIThrowable(const std::exception &e);
    ~JNIThrowable();

    static std::shared_ptr<JNIClass> makeClass();

    std::shared_ptr<JNIObject> toString() override;

private:
    std::string m_message;
};

#endif

#ifndef JAVA_ANDROID_CONTEXT_H
#define JAVA_ANDROID_CONTEXT_H

#include <Java/JNIObject.h>

class AndroidContext final : public JNIObject {
public:
    AndroidContext();
    ~AndroidContext() override;

    static std::shared_ptr<JNIClass> makeClass();
};


#endif


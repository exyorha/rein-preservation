#ifndef JNI_SYSTEM_H
#define JNI_SYSTEM_H

#include <Java/JNIObject.h>

class JNISystem final : public JNIObject {
public:
    static std::shared_ptr<JNIClass> makeClass();

    static int32_t identityHashCode(const std::shared_ptr<JNIObject> &object);
};

#endif


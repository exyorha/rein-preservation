#ifndef JAVA_LILIUM_DEFAULT_ATTEST_CALLBACK_H
#define JAVA_LILIUM_DEFAULT_ATTEST_CALLBACK_H

#include <Java/JNIClass.h>

class LiliumDefaultAttestCallback final : public JNIObject {
public:

    static std::shared_ptr<JNIClass> makeClass();

};

#endif


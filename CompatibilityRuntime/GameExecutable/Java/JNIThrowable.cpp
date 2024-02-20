#include <Java/JNIThrowable.h>
#include <Java/JNIString.h>
#include <Java/JNIClass.h>

JNIThrowable::JNIThrowable() : JNIObject(parent("java/lang/Throwable")) {

}

JNIThrowable::JNIThrowable(const std::exception &e) : JNIObject(parent("java/lang/Throwable")), m_message(e.what()) {

}

JNIThrowable::~JNIThrowable() = default;

std::shared_ptr<JNIObject> JNIThrowable::toString() {
    return std::make_shared<JNIString>(m_message);
}

std::shared_ptr<JNIClass> JNIThrowable::makeClass() {
    auto co = std::make_shared<JNIClass>("java/lang/Throwable", parent("java/lang/Object"));

    return co;
}

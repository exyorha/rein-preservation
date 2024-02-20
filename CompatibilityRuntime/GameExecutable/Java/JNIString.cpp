#include <Java/JNIString.h>
#include <Java/JNIClass.h>

JNIString::JNIString(const std::string_view &string) :
    JNIObject(parent("java/lang/String")),
    m_string(string) {

}

JNIString::~JNIString() = default;

std::shared_ptr<JNIObject> JNIString::toString() {
    return shared_from_this();
}

std::shared_ptr<JNIClass> JNIString::makeClass() {
    auto co = std::make_shared<JNIClass>("java/lang/String", parent("java/lang/Object"));

    return co;
}


#include <Java/JValue.h>
#include <Java/JNIString.h>
#include <Java/JNIState.h>

JValue::operator const std::string &() const {
    auto stringObject = static_cast<std::shared_ptr<JNIString>>(*this);
    if(!stringObject)
        throw std::logic_error("JValue: unexpected null string");

    return stringObject->string();
}

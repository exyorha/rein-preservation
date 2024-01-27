#include <Java/JNIObject.h>
#include <Java/JNIGlobalState.h>
#include <Java/JNIClass.h>
#include <Java/JNIString.h>

#include <sstream>

JNIObject::JNIObject() = default;

JNIObject::~JNIObject() = default;

std::shared_ptr<JNIClass> JNIObject::parent(const std::string_view &name) {
    return JNIGlobalState::get().findClass(name);
}

std::shared_ptr<JNIClass> JNIObject::makeClass() {
    auto co = std::make_shared<JNIClass>("java/lang/Object", nullptr);
    co->registerMethod("toString", "()Ljava/lang/String;", &JNIObject::toString);

    return co;
}

std::shared_ptr<JNIObject> JNIObject::toString(Il2CppArray *args) {
    std::stringstream stream;
    stream << typeid(*this).name() << " ";
    stream << std::hex;
    stream <<reinterpret_cast<uintptr_t>(this);
    return std::make_shared<JNIString>(stream.str());
}

#include <Java/JNIObject.h>
#include <Java/JNIGlobalState.h>
#include <Java/JNIClass.h>
#include <Java/JNIString.h>

#include <sstream>

JNIObject::JNIObject(const std::shared_ptr<JNIClass> &objectClass) : m_objectClass(objectClass) {

}

JNIObject::~JNIObject() = default;

std::shared_ptr<JNIClass> JNIObject::parent(const std::string_view &name) {
    return JNIGlobalState::get().findClass(name);
}

std::shared_ptr<JNIClass> JNIObject::makeClass() {
    auto co = std::make_shared<JNIClass>("java/lang/Object", nullptr);
    co->registerMethod("toString", "()Ljava/lang/String;", &JNIObject::toString);
    co->registerMethod("hashCode", "()I", &JNIObject::hashCode);
    co->registerMethod("getClass", "()Ljava/lang/Object;", &JNIObject::getClass);

    return co;
}

std::shared_ptr<JNIObject> JNIObject::getClass() {
    return objectClass();
}

std::shared_ptr<JNIObject> JNIObject::toString() {
    std::stringstream stream;
    stream << typeid(*this).name() << " ";
    stream << std::hex;
    stream << reinterpret_cast<uintptr_t>(this);
    return std::make_shared<JNIString>(stream.str());
}

void JNIObject::unexpectedNullValue() {
    throw std::runtime_error("unexpected null value");
}

int32_t JNIObject::hashCode() {
    return identityHashCode();
}

int32_t JNIObject::identityHashCode() const {
    return static_cast<int32_t>(std::hash<const void *>()(this));
}

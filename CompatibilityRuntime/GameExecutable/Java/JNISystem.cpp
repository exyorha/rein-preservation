#include <Java/JNISystem.h>
#include <Java/JNIClass.h>
#include <Java/JNIState.h>

std::shared_ptr<JNIClass> JNISystem::makeClass() {
    auto co = std::make_shared<JNIClass>("java/lang/System", parent("java/lang/Object"));
    co->registerMethod("identityHashCode", "(Ljava/lang/Object;)I", &JNISystem::identityHashCode);

    return co;
}


int32_t JNISystem::identityHashCode(const std::shared_ptr<JNIObject> &object) {
    if(!object)
        return 0;

    return object->identityHashCode();
}

#include <Java/JNIUnityReflectionHelper.h>
#include <Java/JNIClass.h>
#include <Java/JNIString.h>
#include <Java/JNIField.h>
#include <Java/JNIState.h>
#include <Java/JNIMethod.h>
#include <Java/JNIString.h>
#include <Java/JNIConstructor.h>

#include <cinttypes>

std::shared_ptr<JNIClass> JNIUnityReflectionHelper::makeClass() {
    auto co = std::make_shared<JNIClass>("com/unity3d/player/ReflectionHelper", parent("java/lang/Object"));
    co->registerMethod("getConstructorID", "(Ljava/lang/Class;Ljava/lang/String;)Ljava/lang/reflect/Constructor;",
                       &JNIUnityReflectionHelper::getConstructorID);
    co->registerMethod("getMethodID", "(Ljava/lang/Class;Ljava/lang/String;Ljava/lang/String;Z)Ljava/lang/reflect/Method;",
                       &JNIUnityReflectionHelper::getMethodID);
    co->registerMethod("getFieldID", "(Ljava/lang/Class;Ljava/lang/String;Ljava/lang/String;Z)Ljava/lang/reflect/Field;",
                       &JNIUnityReflectionHelper::getFieldID);
    co->registerMethod("getFieldSignature", "(Ljava/lang/reflect/Field;)Ljava/lang/String;",
                       &JNIUnityReflectionHelper::getFieldSignature);
    co->registerMethod("newProxyInstance", "(JLjava/lang/Class;)Ljava/lang/Object;", &JNIUnityReflectionHelper::newProxyInstance);
    co->registerMethod("setNativeExceptionOnProxy", "(Ljava/lang/Object;JZ)V", &JNIUnityReflectionHelper::setNativeExceptionOnProxy);
    return co;
}

std::shared_ptr<JNIObject> JNIUnityReflectionHelper::getConstructorID(std::shared_ptr<JNIClass> classPtr,
                                                                      std::shared_ptr<JNIString> signature) {
    nonNull(classPtr);
    nonNull(signature);

    auto constructor = classPtr->getMethodID("<init>", signature->string());

    return std::make_shared<JNIConstructor>(classPtr, *constructor);
}

std::shared_ptr<JNIObject> JNIUnityReflectionHelper::getMethodID(std::shared_ptr<JNIClass> classPtr, std::shared_ptr<JNIString> name,
                                                                 std::shared_ptr<JNIString> signature, bool isStatic) {
    nonNull(classPtr);
    nonNull(name);
    nonNull(signature);

    const JNIClass::RegisteredMethod *method;
    if(isStatic)
        method = classPtr->getStaticMethodID(name->string(), signature->string());
    else
        method = classPtr->getMethodID(name->string(), signature->string());

    return std::make_shared<JNIMethod>(classPtr, *method);
}

std::shared_ptr<JNIObject> JNIUnityReflectionHelper::getFieldID(
    std::shared_ptr<JNIClass> classPtr, std::shared_ptr<JNIString> name, std::shared_ptr<JNIString> signature, bool isStatic) {

    nonNull(classPtr);
    nonNull(name);
    nonNull(signature);

    const JNIClass::RegisteredField *field;
    if(isStatic) {
        field = classPtr->getStaticFieldID(name->string(), signature->string());
    } else {
        field = classPtr->getFieldID(name->string(), signature->string());
    }

    return std::make_shared<JNIField>(classPtr, *field);
}

std::shared_ptr<JNIObject> JNIUnityReflectionHelper::getFieldSignature(std::shared_ptr<JNIField> fieldPtr) {
    nonNull(fieldPtr);

    return std::make_shared<JNIString>(fieldPtr->field()->signature);
}

std::shared_ptr<JNIObject> JNIUnityReflectionHelper::newProxyInstance(int64_t intParam, std::shared_ptr<JNIClass> classPtr) {
    printf("JNIUnityReflectionHelper::newProxyInstance: intParam 0x%" PRIx64 ", class %.*s\n", intParam,
           static_cast<int>(classPtr->name().size()), classPtr->name().data());
    return {};
}

void JNIUnityReflectionHelper::setNativeExceptionOnProxy(std::shared_ptr<JNIObject> objectPtr, int64_t intParam, bool boolParam) {
    printf("JNIUnityReflectionHelper::setNativeExceptionOnProxy: object %p, int param 0x%" PRIx64 ", bool param %d\n",
           objectPtr.get(), intParam, boolParam);
}

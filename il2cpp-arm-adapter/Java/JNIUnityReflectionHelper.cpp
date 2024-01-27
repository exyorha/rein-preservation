#include <Java/JNIUnityReflectionHelper.h>
#include <Java/JNIClass.h>

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

std::shared_ptr<JNIObject> JNIUnityReflectionHelper::getConstructorID(Il2CppArray *args) {
    printf("JNIUnityReflectionHelper::getConstructorID: args %p\n", args);

    printf("JNIUnityReflectionHelper: length of array: %u, byte length of array: %u, size of array header: %u\n",
           il2cpp_array_length(args), il2cpp_array_get_byte_length(args), il2cpp_array_object_header_size());

    auto arrayClass = il2cpp_object_get_class(reinterpret_cast<Il2CppObject *>(args));
    printf("array class name: %s\n",
           il2cpp_class_get_name(arrayClass));

    return {};
}

std::shared_ptr<JNIObject> JNIUnityReflectionHelper::getMethodID(Il2CppArray *args) {
    printf("JNIUnityReflectionHelper::getMethodID: args %p\n", args);

    printf("JNIUnityReflectionHelper: length of array: %u, byte length of array: %u, size of array header: %u\n",
           il2cpp_array_length(args), il2cpp_array_get_byte_length(args), il2cpp_array_object_header_size());

    auto arrayClass = il2cpp_object_get_class(reinterpret_cast<Il2CppObject *>(args));
    printf("array class name: %s\n",
           il2cpp_class_get_name(arrayClass));

    return {};
}

std::shared_ptr<JNIObject> JNIUnityReflectionHelper::getFieldID(Il2CppArray *args) {
    printf("JNIUnityReflectionHelper::getFieldID: args %p\n", args);

    printf("JNIUnityReflectionHelper: length of array: %u, byte length of array: %u, size of array header: %u\n",
           il2cpp_array_length(args), il2cpp_array_get_byte_length(args), il2cpp_array_object_header_size());

    auto arrayClass = il2cpp_object_get_class(reinterpret_cast<Il2CppObject *>(args));
    printf("array class name: %s\n",
           il2cpp_class_get_name(arrayClass));

    return {};
}

std::shared_ptr<JNIObject> JNIUnityReflectionHelper::getFieldSignature(Il2CppArray *args) {
    printf("JNIUnityReflectionHelper::getFieldSignature: args %p\n", args);
    return {};
}

std::shared_ptr<JNIObject> JNIUnityReflectionHelper::newProxyInstance(Il2CppArray *args) {
    printf("JNIUnityReflectionHelper::getFieldSignature: args %p\n", args);
    return {};
}

void JNIUnityReflectionHelper::setNativeExceptionOnProxy(Il2CppArray *args) {
    printf("JNIUnityReflectionHelper::setNativeExceptionOnProxy: args %p\n", args);
}

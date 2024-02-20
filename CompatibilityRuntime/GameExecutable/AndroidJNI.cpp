#include <translator_api.h>
#include "Il2CppUtilities.h"
#include "Java/JNIConstructor.h"
#include "Java/JValue.h"

#include <Java/JNIState.h>
#include <Java/JNIGlobalState.h>
#include <Java/JNIClass.h>
#include <Java/JNIString.h>
#include <Java/JNIExecutable.h>
#include <Java/JNIField.h>

#if 0
static intptr_t AndroidJNI_AttachCurrentThread(intptr_t (*original)(void))
INTERPOSE_ICALL("UnityEngine.AndroidJNI::AttachCurrentThread", AndroidJNI_AttachCurrentThread)

static intptr_t AndroidJNI_DetachCurrentThread(intptr_t (*original)(void));
INTERPOSE_ICALL("UnityEngine.AndroidJNI::DetachCurrentThread", AndroidJNI_DetachCurrentThread)

static int32_t AndroidJNI_GetVersion(int32_t (*original)(void));
INTERPOSE_ICALL("UnityEngine.AndroidJNI::GetVersion", AndroidJNI_GetVersion);
#endif

static intptr_t AndroidJNI_FindClass(Il2CppString *name,
                                     intptr_t (*original)(Il2CppString *name)) {

    (void)original;

    auto &jni = JNIState::get();
    return jni.guard([name, &jni]() -> intptr_t {
        return jni.makeLocalReference(JNIGlobalState::get().findClass(stringToUtf8(name)));
    });
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::FindClass", AndroidJNI_FindClass);

static intptr_t AndroidJNI_ExceptionOccurred(intptr_t (*original)()) {
    auto &jni = JNIState::get();

    return jni.makeLocalReference(jni.exceptionOccurred());
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::ExceptionOccurred", AndroidJNI_ExceptionOccurred);

static void AndroidJNI_ExceptionClear(void (*original)()) {
    JNIState::get().exceptionClear();
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::ExceptionClear", AndroidJNI_ExceptionClear);

static void AndroidJNI_DeleteLocalRef(intptr_t ref, void (*original)(intptr_t)) {
    JNIState::get().deleteLocalRef(ref);
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::DeleteLocalRef", AndroidJNI_DeleteLocalRef);

static intptr_t AndroidJNI_NewGlobalRef(intptr_t ref, intptr_t (*original)(intptr_t)) {
    auto object = JNIState::get().resolve<JNIObject>(ref);
    return JNIGlobalState::get().newGlobalRef(object);
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::NewGlobalRef", AndroidJNI_NewGlobalRef);

static void AndroidJNI_DeleteGlobalRef(intptr_t ref, void *original) {
    JNIGlobalState::get().deleteGlobalRef(ref);
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::DeleteGlobalRef", AndroidJNI_DeleteGlobalRef);

static intptr_t AndroidJNI_GetMethodID(intptr_t classRef, Il2CppString *methodName, Il2CppString *methodSignature,
                                       intptr_t (*original)(intptr_t classRef, Il2CppString *methodName, Il2CppString *methodSignature)) {

    auto &jni = JNIState::get();
    return jni.guard([classRef, &jni, methodName, methodSignature]() -> intptr_t {
        auto object = jni.resolveNonNull<JNIClass>(classRef);
        return reinterpret_cast<intptr_t>(object->getMethodID(stringToUtf8(methodName), stringToUtf8(methodSignature)));
    });
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::GetMethodID", AndroidJNI_GetMethodID);

static intptr_t AndroidJNI_GetStaticMethodID(intptr_t classRef, Il2CppString *methodName, Il2CppString *methodSignature,
                                       intptr_t (*original)(intptr_t classRef, Il2CppString *methodName, Il2CppString *methodSignature)) {

    auto &jni = JNIState::get();
    return jni.guard([classRef, &jni, methodName, methodSignature]() -> intptr_t {
        auto object = jni.resolveNonNull<JNIClass>(classRef);
        return reinterpret_cast<intptr_t>(object->getStaticMethodID(stringToUtf8(methodName), stringToUtf8(methodSignature)));
    });
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::GetStaticMethodID", AndroidJNI_GetStaticMethodID);

static intptr_t AndroidJNI_GetFieldID(intptr_t classRef, Il2CppString *fieldName, Il2CppString *fieldSignature,
                                      void *original) {

    auto &jni = JNIState::get();
    return jni.guard([classRef, &jni, fieldName, fieldSignature]() -> intptr_t {
        auto object = jni.resolveNonNull<JNIClass>(classRef);
        return reinterpret_cast<intptr_t>(object->getFieldID(stringToUtf8(fieldName), stringToUtf8(fieldSignature)));
    });
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::GetFieldID", AndroidJNI_GetFieldID);

static intptr_t AndroidJNI_GetStaticFieldID(intptr_t classRef, Il2CppString *fieldName, Il2CppString *fieldSignature,
                                            void *original) {

    auto &jni = JNIState::get();
    return jni.guard([classRef, &jni, fieldName, fieldSignature]() -> intptr_t {
        auto object = jni.resolveNonNull<JNIClass>(classRef);
        return reinterpret_cast<intptr_t>(object->getStaticFieldID(stringToUtf8(fieldName), stringToUtf8(fieldSignature)));
    });
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::GetStaticFieldID", AndroidJNI_GetStaticFieldID);

static Il2CppString *AndroidJNI_CallStringMethod(intptr_t objectRef, intptr_t methodID, Il2CppArray *argsPtr,
                                                 Il2CppString *(*original)(intptr_t objectRef, intptr_t methodID, Il2CppArray *args)) {

    auto &jni = JNIState::get();
    return jni.guard([&jni, objectRef, methodID, argsPtr]() -> Il2CppString * {
        auto object = jni.resolveNonNull<JNIObject>(objectRef);
        if(methodID == 0) {
            throw std::runtime_error("attempted to call a null method ID");
        }

        auto &invokable = reinterpret_cast<const JNIClass::RegisteredMethod *>(methodID)->invokable;
        auto &function = std::get<JNIObjectMethod>(invokable);

        JValueArray args(argsPtr);

        auto result = function->invoke(*object, args);

        if(!result)
            return nullptr;

        auto jniString = std::dynamic_pointer_cast<JNIString>(result);
        if(!jniString)
            throw std::logic_error("the result of a String method is not a string");

        return stringFromUtf8(jniString->string());
    });
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::CallStringMethod", AndroidJNI_CallStringMethod);

static intptr_t AndroidJNI_CallObjectMethod(intptr_t objectRef, intptr_t methodID, Il2CppArray *argsPtr,
                                            void *original) {

    auto &jni = JNIState::get();
    return jni.guard([&jni, objectRef, methodID, argsPtr]() -> intptr_t {
        auto object = jni.resolveNonNull<JNIObject>(objectRef);
        if(methodID == 0) {
            throw std::runtime_error("attempted to call a null method ID");
        }

        auto &invokable = reinterpret_cast<const JNIClass::RegisteredMethod *>(methodID)->invokable;
        auto &function = std::get<JNIObjectMethod>(invokable);

        JValueArray args(argsPtr);

        return jni.makeLocalReference(function->invoke(*object, args));
    });
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::CallObjectMethod", AndroidJNI_CallObjectMethod);

static void AndroidJNI_CallVoidMethod(intptr_t objectRef, intptr_t methodID, Il2CppArray *argsPtr,
                                          void *original) {

    auto &jni = JNIState::get();
    jni.guard([&jni, objectRef, methodID, argsPtr]() {
        auto object = jni.resolveNonNull<JNIObject>(objectRef);
        if(methodID == 0) {
            throw std::runtime_error("attempted to call a null method ID");
        }

        auto &invokable = reinterpret_cast<const JNIClass::RegisteredMethod *>(methodID)->invokable;
        auto &function = std::get<JNIVoidMethod>(invokable);

        JValueArray args(argsPtr);

        function->invoke(*object, args);
    });
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::CallVoidMethod", AndroidJNI_CallVoidMethod);

static float AndroidJNI_CallFloatMethod(intptr_t objectRef, intptr_t methodID, Il2CppArray *argsPtr,
                                          void *original) {

    auto &jni = JNIState::get();
    return jni.guard([&jni, objectRef, methodID, argsPtr]() -> float {
        auto object = jni.resolveNonNull<JNIObject>(objectRef);
        if(methodID == 0) {
            throw std::runtime_error("attempted to call a null method ID");
        }

        auto &invokable = reinterpret_cast<const JNIClass::RegisteredMethod *>(methodID)->invokable;
        auto &function = std::get<JNIFloatMethod>(invokable);

        JValueArray args(argsPtr);

        return function->invoke(*object, args);
    });
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::CallFloatMethod", AndroidJNI_CallFloatMethod);

static int32_t AndroidJNI_CallIntMethod(intptr_t objectRef, intptr_t methodID, Il2CppArray *argsPtr,
                                          void *original) {

    auto &jni = JNIState::get();
    return jni.guard([&jni, objectRef, methodID, argsPtr]() -> int32_t {
        auto object = jni.resolveNonNull<JNIObject>(objectRef);
        if(methodID == 0) {
            throw std::runtime_error("attempted to call a null method ID");
        }

        auto &invokable = reinterpret_cast<const JNIClass::RegisteredMethod *>(methodID)->invokable;
        auto &function = std::get<JNIIntMethod>(invokable);

        JValueArray args(argsPtr);

        return function->invoke(*object, args);
    });
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::CallIntMethod", AndroidJNI_CallIntMethod);

static int64_t AndroidJNI_CallLongMethod(intptr_t objectRef, intptr_t methodID, Il2CppArray *argsPtr,
                                          void *original) {

    auto &jni = JNIState::get();
    return jni.guard([&jni, objectRef, methodID, argsPtr]() -> int64_t {
        auto object = jni.resolveNonNull<JNIObject>(objectRef);
        if(methodID == 0) {
            throw std::runtime_error("attempted to call a null method ID");
        }

        auto &invokable = reinterpret_cast<const JNIClass::RegisteredMethod *>(methodID)->invokable;
        auto &function = std::get<JNILongMethod>(invokable);

        JValueArray args(argsPtr);

        return function->invoke(*object, args);
    });
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::CallLongMethod", AndroidJNI_CallLongMethod);

static bool AndroidJNI_CallBooleanMethod(intptr_t objectRef, intptr_t methodID, Il2CppArray *argsPtr,
                                          void *original) {

    auto &jni = JNIState::get();
    return jni.guard([&jni, objectRef, methodID, argsPtr]() -> bool {
        auto object = jni.resolveNonNull<JNIObject>(objectRef);
        if(methodID == 0) {
            throw std::runtime_error("attempted to call a null method ID");
        }

        auto &invokable = reinterpret_cast<const JNIClass::RegisteredMethod *>(methodID)->invokable;
        auto &function = std::get<JNIBooleanMethod>(invokable);

        JValueArray args(argsPtr);

        return function->invoke(*object, args);
    });
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::CallBooleanMethod", AndroidJNI_CallBooleanMethod);

static Il2CppString *AndroidJNI_CallStaticStringMethod(intptr_t classRef, intptr_t methodID, Il2CppArray *argsPtr,
                                                 Il2CppString *(*original)(intptr_t objectRef, intptr_t methodID, Il2CppArray *args)) {

    auto &jni = JNIState::get();
    return jni.guard([&jni, methodID, argsPtr]() -> Il2CppString * {
        if(methodID == 0) {
            throw std::runtime_error("attempted to call a null method ID");
        }

        auto &invokable = reinterpret_cast<const JNIClass::RegisteredMethod *>(methodID)->invokable;
        auto &function = std::get<JNIStaticObjectMethod>(invokable);if(methodID == 0) {
            throw std::runtime_error("attempted to call a null method ID");
        }

        JValueArray args(argsPtr);
        auto result = function->invoke(args);

        if(!result)
            return nullptr;

        auto jniString = std::dynamic_pointer_cast<JNIString>(result);
        if(!jniString)
            throw std::logic_error("the result of a String method is not a string");

        return stringFromUtf8(jniString->string());
    });
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::CallStaticStringMethod", AndroidJNI_CallStaticStringMethod);

static intptr_t AndroidJNI_CallStaticObjectMethod(intptr_t classRef, intptr_t methodID, Il2CppArray *argsPtr,
                                                 intptr_t (*original)(intptr_t objectRef, intptr_t methodID, Il2CppArray *args)) {

    auto &jni = JNIState::get();
    return jni.guard([&jni, methodID, argsPtr]() -> intptr_t {
        if(methodID == 0) {
            throw std::runtime_error("attempted to call a null method ID");
        }

        auto &invokable = reinterpret_cast<const JNIClass::RegisteredMethod *>(methodID)->invokable;
        auto &function = std::get<JNIStaticObjectMethod>(invokable);

        JValueArray args(argsPtr);
        return jni.makeLocalReference(function->invoke(args));
    });
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::CallStaticObjectMethod", AndroidJNI_CallStaticObjectMethod);

static float AndroidJNI_CallStaticFloatMethod(intptr_t classRef, intptr_t methodID, Il2CppArray *argsPtr,
                                                 float (*original)(intptr_t objectRef, intptr_t methodID, Il2CppArray *args)) {

    auto &jni = JNIState::get();
    return jni.guard([&jni, methodID, argsPtr]() -> intptr_t {
        if(methodID == 0) {
            throw std::runtime_error("attempted to call a null method ID");
        }

        auto &invokable = reinterpret_cast<const JNIClass::RegisteredMethod *>(methodID)->invokable;
        auto &function = std::get<JNIStaticFloatMethod>(invokable);

        JValueArray args(argsPtr);
        return function->invoke(args);
    });
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::CallStaticFloatMethod", AndroidJNI_CallStaticFloatMethod);

static int32_t AndroidJNI_CallStaticIntMethod(intptr_t classRef, intptr_t methodID, Il2CppArray *argsPtr,
                                              int32_t (*original)(intptr_t objectRef, intptr_t methodID, Il2CppArray *args)) {

    auto &jni = JNIState::get();
    return jni.guard([&jni, methodID, argsPtr]() -> int32_t {
        if(methodID == 0) {
            throw std::runtime_error("attempted to call a null method ID");
        }

        auto &invokable = reinterpret_cast<const JNIClass::RegisteredMethod *>(methodID)->invokable;
        auto &function = std::get<JNIStaticIntMethod>(invokable);

        JValueArray args(argsPtr);
        return function->invoke(args);
    });
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::CallStaticIntMethod", AndroidJNI_CallStaticIntMethod);

static int64_t AndroidJNI_CallStaticLongMethod(intptr_t classRef, intptr_t methodID, Il2CppArray *argsPtr,
                                              int64_t (*original)(intptr_t objectRef, intptr_t methodID, Il2CppArray *args)) {

    auto &jni = JNIState::get();
    return jni.guard([&jni, methodID, argsPtr]() -> int64_t {
        if(methodID == 0) {
            throw std::runtime_error("attempted to call a null method ID");
        }

        auto &invokable = reinterpret_cast<const JNIClass::RegisteredMethod *>(methodID)->invokable;
        auto &function = std::get<JNIStaticLongMethod>(invokable);

        JValueArray args(argsPtr);
        return function->invoke(args);
    });
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::CallStaticLongMethod", AndroidJNI_CallStaticLongMethod);

static bool AndroidJNI_CallStaticBooleanMethod(intptr_t classRef, intptr_t methodID, Il2CppArray *argsPtr,
                                                 bool (*original)(intptr_t objectRef, intptr_t methodID, Il2CppArray *args)) {

    auto &jni = JNIState::get();
    return jni.guard([&jni, methodID, argsPtr]() -> bool {
        if(methodID == 0) {
            throw std::runtime_error("attempted to call a null method ID");
        }

        auto &invokable = reinterpret_cast<const JNIClass::RegisteredMethod *>(methodID)->invokable;
        auto &function = std::get<JNIStaticBooleanMethod>(invokable);

        JValueArray args(argsPtr);
        return function->invoke(args);
    });
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::CallStaticBooleanMethod", AndroidJNI_CallStaticBooleanMethod);

static void AndroidJNI_CallStaticVoidMethod(intptr_t classRef, intptr_t methodID, Il2CppArray *argsPtr,
                                            void *original) {

    auto &jni = JNIState::get();
    jni.guard([&jni, methodID, argsPtr]() {
        if(methodID == 0) {
            throw std::runtime_error("attempted to call a null method ID");
        }

        auto &invokable = reinterpret_cast<const JNIClass::RegisteredMethod *>(methodID)->invokable;
        auto &function = std::get<JNIStaticVoidMethod>(invokable);

        JValueArray args(argsPtr);
        function->invoke(args);
    });
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::CallStaticVoidMethod", AndroidJNI_CallStaticVoidMethod);

static int32_t AndroidJNI_PushLocalFrame(int32_t capacity, int32_t (*original)(int32_t)) {
    (void)capacity;
    JNIState::get().pushLocalFrame();
    return 0;
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::PushLocalFrame", AndroidJNI_PushLocalFrame);

static intptr_t AndroidJNI_PopLocalFrame(intptr_t objectToChain, intptr_t (*original)(intptr_t)) {

    auto &jni = JNIState::get();
    return jni.guard([&jni, objectToChain]() -> intptr_t {

        auto object = jni.resolve<JNIObject>(objectToChain);

        jni.popLocalFrame();

        return jni.makeLocalReference(object);
    });
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::PopLocalFrame", AndroidJNI_PopLocalFrame);

static intptr_t AndroidJNI_NewStringFromStr(Il2CppString *string, intptr_t (*original)(Il2CppString *)) {

    auto &jni = JNIState::get();
    return jni.guard([&jni, string]() -> intptr_t {

        std::shared_ptr<JNIString> jstring;

        if(string) {
            jstring = std::make_shared<JNIString>(stringToUtf8(string));
        }

        return jni.makeLocalReference(jstring);
    });
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::NewStringFromStr", AndroidJNI_NewStringFromStr);

static intptr_t AndroidJNI_FromReflectedMethod(intptr_t reflectedMethod, void *original) {

    auto &jni = JNIState::get();
    return jni.guard([&jni, reflectedMethod]() -> intptr_t {
        auto method = jni.resolve<JNIExecutable>(reflectedMethod);
        if(!method)
            return 0;

        return reinterpret_cast<intptr_t>(method->method());
    });
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::FromReflectedMethod", AndroidJNI_FromReflectedMethod);

static intptr_t AndroidJNI_FromReflectedField(intptr_t reflectedField, void *original) {

    auto &jni = JNIState::get();
    return jni.guard([&jni, reflectedField]() -> intptr_t {
        auto field = jni.resolve<JNIField>(reflectedField);
        if(!field)
            return 0;

        return reinterpret_cast<intptr_t>(field->field());
    });
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::FromReflectedField", AndroidJNI_FromReflectedField);

static intptr_t AndroidJNI_NewObject(intptr_t classRef, intptr_t methodID, Il2CppArray *argsPtr,
                                     void *original) {

    auto &jni = JNIState::get();
    return jni.guard([&jni, methodID, argsPtr]() -> intptr_t {
        if(methodID == 0) {
            throw std::runtime_error("attempted to create an object with a null method ID as the constructor");
        }

        auto &invokable = reinterpret_cast<const JNIClass::RegisteredMethod *>(methodID)->invokable;
        auto &function = std::get<JNIConstructorMethod>(invokable);

        JValueArray args(argsPtr);
        return jni.makeLocalReference(function->invoke(args));
    });
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::NewObject", AndroidJNI_NewObject);

static intptr_t AndroidJNI_GetObjectClass(intptr_t objectRef, void *original) {
    auto &jni = JNIState::get();
    return jni.guard([&jni, objectRef]() -> intptr_t {
        auto object = jni.resolveNonNull<JNIObject>(objectRef);
        return jni.makeLocalReference(object->objectClass());
    });
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::GetObjectClass", AndroidJNI_GetObjectClass);

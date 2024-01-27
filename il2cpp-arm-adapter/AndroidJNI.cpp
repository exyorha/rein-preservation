#include <ICall/ICallInterposerManager.h>
#include "Il2CppUtilities.h"

#include <Java/JNIState.h>
#include <Java/JNIGlobalState.h>
#include <Java/JNIClass.h>
#include <Java/JNIString.h>

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
        printf("AndroidJNI::FindClass(%s)\n", stringToUtf8(name).c_str());

        return jni.makeLocalReference(JNIGlobalState::get().findClass(stringToUtf8(name)));
    });
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::FindClass", AndroidJNI_FindClass);

static intptr_t AndroidJNI_ExceptionOccurred(intptr_t (*original)()) {
    auto &jni = JNIState::get();

    auto result = jni.makeLocalReference(jni.exceptionOccurred());

    printf("AndroidJNI_ExceptionOccurred: 0x%016" PRIx64 "\n",result);
    return result;
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::ExceptionOccurred", AndroidJNI_ExceptionOccurred);

static void AndroidJNI_ExceptionClear(void (*original)()) {
    printf("AndroidJNI_ExceptionClear\n");

    JNIState::get().exceptionClear();
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::ExceptionClear", AndroidJNI_ExceptionClear);

static void AndroidJNI_DeleteLocalRef(intptr_t ref, void (*original)(intptr_t)) {
    printf("AndroidJNI_DeleteLocalRef(%" PRId64 ")\n", ref);
    JNIState::get().deleteLocalRef(ref);
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::DeleteLocalRef", AndroidJNI_DeleteLocalRef);

static intptr_t AndroidJNI_NewGlobalRef(intptr_t ref, intptr_t (*original)(intptr_t)) {
    printf("AndroidJNI_NewGlobalRef(%" PRId64 ")\n", ref);

    auto object = JNIState::get().resolve<JNIObject>(ref);
    return JNIGlobalState::get().newGlobalRef(object);
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::NewGlobalRef", AndroidJNI_NewGlobalRef);

static void AndroidJNI_DeleteGlobalRef(intptr_t ref) {
    printf("AndroidJNI_DeleteGlobalRef(%" PRId64 ")\n", ref);

    JNIGlobalState::get().deleteGlobalRef(ref);
}

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

static Il2CppString *AndroidJNI_CallStringMethod(intptr_t objectRef, intptr_t methodID, Il2CppArray *args,
                                                 Il2CppString *(*original)(intptr_t objectRef, intptr_t methodID, Il2CppArray *args)) {

    auto &jni = JNIState::get();
    return jni.guard([&jni, objectRef, methodID, args]() -> Il2CppString * {
        auto object = jni.resolveNonNull<JNIObject>(objectRef);
        auto &invokable = *reinterpret_cast<const MethodInvokable *>(methodID);
        auto function = std::get<JNIObjectMethod>(invokable);

        auto result = (object.get()->*function)(args);

        if(!result)
            return nullptr;

        auto jniString = std::dynamic_pointer_cast<JNIString>(result);
        if(!jniString)
            throw std::logic_error("the result of a String method is not a string");

        auto resultStr = stringFromUtf8(jniString->string());

        printf("CallStringMethod returning %p\n", resultStr);

        return resultStr;
    });
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::CallStringMethod", AndroidJNI_CallStringMethod);

static Il2CppString *AndroidJNI_CallStaticStringMethod(intptr_t classRef, intptr_t methodID, Il2CppArray *args,
                                                 Il2CppString *(*original)(intptr_t objectRef, intptr_t methodID, Il2CppArray *args)) {

    auto &jni = JNIState::get();
    return jni.guard([&jni, methodID, args]() -> Il2CppString * {
        auto &invokable = *reinterpret_cast<const MethodInvokable *>(methodID);
        auto function = std::get<JNIStaticObjectMethod>(invokable);

        auto result = function(args);

        if(!result)
            return nullptr;

        auto jniString = std::dynamic_pointer_cast<JNIString>(result);
        if(!jniString)
            throw std::logic_error("the result of a String method is not a string");

        auto resultStr = stringFromUtf8(jniString->string());

        printf("CallStaticStringMethod returning %p\n", resultStr);

        return resultStr;
    });
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::CallStaticStringMethod", AndroidJNI_CallStaticStringMethod);

static intptr_t AndroidJNI_CallStaticObjectMethod(intptr_t classRef, intptr_t methodID, Il2CppArray *args,
                                                 intptr_t (*original)(intptr_t objectRef, intptr_t methodID, Il2CppArray *args)) {

    auto &jni = JNIState::get();
    return jni.guard([&jni, methodID, args]() -> intptr_t {
        auto &invokable = *reinterpret_cast<const MethodInvokable *>(methodID);
        auto function = std::get<JNIStaticObjectMethod>(invokable);

        return jni.makeLocalReference(function(args));
    });
}
INTERPOSE_ICALL("UnityEngine.AndroidJNI::CallStaticObjectMethod", AndroidJNI_CallStaticObjectMethod);

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

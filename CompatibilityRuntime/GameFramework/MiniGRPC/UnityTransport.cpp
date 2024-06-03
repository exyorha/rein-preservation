#include <MiniGRPC/CompletionQueue.h>
#include <MiniGRPC/grpcsharp.h>
#include <MiniGRPC/CallExecutor.h>

#include <translator_api.h>

#include <stdexcept>
#include <cstdio>

namespace minigrpc {

    using UnityUpdateFunction = void (*)();

    // mirrors UnityEngine.LowLevel#PlayerLoopSystemInternal
    struct UnityPlayerLoopSystemInternal {
        void *type;
        void *updateDelegate;
        UnityUpdateFunction *updateFunction;
        void *loopConditionFunction;
        int32_t numSubSystems;
    };
    static_assert(sizeof(UnityPlayerLoopSystemInternal) == 40);

    CallExecutor callExecutor;

    Il2CppClass * UnityWebRequestClass;
    const MethodInfo *UnityWebRequest_ctor;
    const MethodInfo *UnityWebRequest_SetRequestHeader;
    const MethodInfo *UnityWebRequest_SendWebRequest;
    const MethodInfo *UnityWebRequest_Dispose;
    const MethodInfo *UnityWebRequest_GetResponseHeader;
    PropertyInfo *UnityWebRequest_isDone;
    const MethodInfo *UnityWebRequest_isDone_get;
    PropertyInfo *UnityWebRequest_error;
    const MethodInfo *UnityWebRequest_error_get;
    PropertyInfo *UnityWebRequest_responseCode;
    const MethodInfo *UnityWebRequest_responseCode_get;

    UnityWebRequest_GetResponseHeaderKeys_Type UnityWebRequest_GetResponseHeaderKeys;

    Il2CppClass * DownloadHandlerBufferClass;
    const MethodInfo *DownloadHandlerBuffer_ctor;
    PropertyInfo *DownloadHandlerBuffer_text;
    const MethodInfo *DownloadHandlerBuffer_text_get;
    PropertyInfo *DownloadHandlerBuffer_data;
    const MethodInfo *DownloadHandlerBuffer_data_get;

    Il2CppClass * UploadHandlerRawClass;
    const MethodInfo *UploadHandlerRaw_ctor;

    Il2CppClass *SystemByteClass;

    int32_t ArrayHeaderSize;

    static void minigrpcUpdate() {
        callExecutor.update();
    }

    void platformInitialize() {
        static std::atomic_bool initializedBefore;
        if(initializedBefore.exchange(true))
            return;

        /*
         * Find the relevant classes and methods.
         */
        auto unityWebRequestAssembly = il2cpp_domain_assembly_open(il2cpp_domain_get(), "UnityEngine.UnityWebRequestModule.dll");
        if(!unityWebRequestAssembly)
            throw std::runtime_error("the WebRequest assembly was not found");

        auto unityWebRequestImage = il2cpp_assembly_get_image(unityWebRequestAssembly);

        UnityWebRequestClass = il2cpp_class_from_name(unityWebRequestImage, "UnityEngine.Networking", "UnityWebRequest");
        if(!UnityWebRequestClass)
            throw std::runtime_error("UnityWebRequest class was not found");

        UnityWebRequest_ctor = il2cpp_class_get_method_from_name(UnityWebRequestClass, ".ctor", 4);
        UnityWebRequest_SetRequestHeader = il2cpp_class_get_method_from_name(UnityWebRequestClass, "SetRequestHeader", 2);
        UnityWebRequest_SendWebRequest = il2cpp_class_get_method_from_name(UnityWebRequestClass, "SendWebRequest", 0);
        UnityWebRequest_Dispose = il2cpp_class_get_method_from_name(UnityWebRequestClass, "Dispose", 0);
        UnityWebRequest_GetResponseHeader = il2cpp_class_get_method_from_name(UnityWebRequestClass, "GetResponseHeader", 1);

        UnityWebRequest_isDone = const_cast<PropertyInfo *>(il2cpp_class_get_property_from_name(UnityWebRequestClass, "isDone"));
        UnityWebRequest_isDone_get = il2cpp_property_get_get_method(UnityWebRequest_isDone);
        UnityWebRequest_error = const_cast<PropertyInfo *>(il2cpp_class_get_property_from_name(UnityWebRequestClass, "error"));
        UnityWebRequest_error_get = il2cpp_property_get_get_method(UnityWebRequest_error);
        UnityWebRequest_responseCode =
            const_cast<PropertyInfo *>(il2cpp_class_get_property_from_name(UnityWebRequestClass, "responseCode"));
        UnityWebRequest_responseCode_get = il2cpp_property_get_get_method(UnityWebRequest_responseCode);

        DownloadHandlerBufferClass = il2cpp_class_from_name(unityWebRequestImage, "UnityEngine.Networking", "DownloadHandlerBuffer");
        if(!DownloadHandlerBufferClass)
            throw std::runtime_error("DownloadHandlerBuffer class was not found");

        DownloadHandlerBuffer_ctor = il2cpp_class_get_method_from_name(DownloadHandlerBufferClass, ".ctor", 0);

        DownloadHandlerBuffer_text = const_cast<PropertyInfo *>(il2cpp_class_get_property_from_name(DownloadHandlerBufferClass, "text"));
        DownloadHandlerBuffer_text_get = il2cpp_property_get_get_method(DownloadHandlerBuffer_text);

        DownloadHandlerBuffer_data = const_cast<PropertyInfo *>(il2cpp_class_get_property_from_name(DownloadHandlerBufferClass, "data"));
        DownloadHandlerBuffer_data_get = il2cpp_property_get_get_method(DownloadHandlerBuffer_data);

        UnityWebRequest_GetResponseHeaderKeys = reinterpret_cast<UnityWebRequest_GetResponseHeaderKeys_Type>(
            translator_resolve_native_icall("UnityEngine.Networking.UnityWebRequest::GetResponseHeaderKeys"));

        UploadHandlerRawClass = il2cpp_class_from_name(unityWebRequestImage, "UnityEngine.Networking", "UploadHandlerRaw");
        if(!DownloadHandlerBufferClass)
            throw std::runtime_error("UploadHandlerRawClass class was not found");

        UploadHandlerRaw_ctor = il2cpp_class_get_method_from_name(UploadHandlerRawClass, ".ctor", 1);

        SystemByteClass = il2cpp_class_from_name(il2cpp_get_corlib(), "System", "Byte");
        if(!SystemByteClass)
            throw std::runtime_error("System.Byte class was not found");

        ArrayHeaderSize = il2cpp_array_object_header_size();

        /*
         * Install our update handler into Unity's player loop.
         */
        using GetCurrentPlayerLoopInternalFunc = Il2CppArray *(*)();

        using SetPlayerLoopInternalFunc = void (*)(Il2CppArray *loop);

        GetCurrentPlayerLoopInternalFunc GetCurrentPlayerLoopInternal =
            reinterpret_cast<GetCurrentPlayerLoopInternalFunc>(
                translator_resolve_native_icall("UnityEngine.LowLevel.PlayerLoop::GetCurrentPlayerLoopInternal"));

        SetPlayerLoopInternalFunc SetPlayerLoopInternal =
            reinterpret_cast<SetPlayerLoopInternalFunc>(
                translator_resolve_native_icall("UnityEngine.LowLevel.PlayerLoop::SetPlayerLoopInternal"));

        auto currentLoop = GetCurrentPlayerLoopInternal();
        auto numberOfElements = il2cpp_array_length(currentLoop);
        auto bytesize = il2cpp_array_get_byte_length(currentLoop);

        auto newLoop = il2cpp_array_new_specific(
            il2cpp_object_get_class(reinterpret_cast<Il2CppObject *>(currentLoop)),
            numberOfElements + 1);

        auto headerSize = il2cpp_array_object_header_size();

        auto originalElements = reinterpret_cast<UnityPlayerLoopSystemInternal *>(
            reinterpret_cast<unsigned char *>(currentLoop) + headerSize);

        auto newElements = reinterpret_cast<UnityPlayerLoopSystemInternal *>(
            reinterpret_cast<unsigned char *>(newLoop) + headerSize);

        memcpy(newElements, originalElements, sizeof(UnityPlayerLoopSystemInternal) * numberOfElements);

        auto &newSystem = newElements[numberOfElements];

        if(newElements[0].numSubSystems != numberOfElements - 1)
            throw std::runtime_error("unexpected numSubSystems of the first system");

        static UnityUpdateFunction minigrpcUpdatePtr = minigrpcUpdate;

        newElements[0].numSubSystems++;
        newSystem.type = nullptr;
        newSystem.updateDelegate = nullptr;
        newSystem.updateFunction = &minigrpcUpdatePtr;
        newSystem.loopConditionFunction = nullptr;
        newSystem.numSubSystems = 0;

        SetPlayerLoopInternal(newLoop);
    }

    void platformFinalize() {

    }
}


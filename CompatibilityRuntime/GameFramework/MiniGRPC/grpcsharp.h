#ifndef GRPCSHARP_H
#define GRPCSHARP_H

#include "il2cpp-api-types.h"
#include <grpc_csharp_ext.h>
#include <translator_api.h>

namespace minigrpc {
    class CallExecutor;

    extern grpcsharp_log_func logFunction;
    extern grpcsharp_native_callback_dispatcher_func nativeCallbackDispatcher;

    void platformInitialize();
    void platformFinalize();

    extern CallExecutor callExecutor;

    extern Il2CppClass * UnityWebRequestClass;
    extern const MethodInfo *UnityWebRequest_ctor;
    extern const MethodInfo *UnityWebRequest_SetRequestHeader;
    extern const MethodInfo *UnityWebRequest_SendWebRequest;
    extern const MethodInfo *UnityWebRequest_Dispose;
    extern const MethodInfo *UnityWebRequest_GetResponseHeader;
    extern PropertyInfo *UnityWebRequest_isDone;
    extern const MethodInfo *UnityWebRequest_isDone_get;
    extern PropertyInfo *UnityWebRequest_error;
    extern const MethodInfo *UnityWebRequest_error_get;
    extern PropertyInfo *UnityWebRequest_responseCode;
    extern const MethodInfo *UnityWebRequest_responseCode_get;

    extern Il2CppClass *DownloadHandlerBufferClass;
    extern const MethodInfo *DownloadHandlerBuffer_ctor;
    extern PropertyInfo *DownloadHandlerBuffer_text;
    extern const MethodInfo *DownloadHandlerBuffer_text_get;
    extern PropertyInfo *DownloadHandlerBuffer_data;
    extern const MethodInfo *DownloadHandlerBuffer_data_get;

    extern Il2CppClass *UploadHandlerRawClass;
    extern const MethodInfo *UploadHandlerRaw_ctor;

    extern Il2CppClass *SystemByteClass;

    using UnityWebRequest_GetResponseHeaderKeys_Type = Il2CppArray *(*)(Il2CppObject *this_);
    extern UnityWebRequest_GetResponseHeaderKeys_Type UnityWebRequest_GetResponseHeaderKeys;

    extern int32_t ArrayHeaderSize;
}

#endif

#include "ExecutingCall.h"
#include "CompletionQueue.h"
#include "BatchContext.h"

#include <Il2CppUtilities.h>

#include "grpcsharp.h"
#include "il2cpp-api-types.h"
#include "translator_api.h"

#include <stdexcept>
#include <array>
#include <sstream>

namespace minigrpc {

    ExecutingCall::ExecutingCall(
        const std::string &url,
        CompletionQueue *cq,
        BatchContext *batch,
        std::vector<unsigned char> &&requestData,
        std::vector<std::pair<std::string, std::string>> &&requestMetadata
    ) :
        m_cq(cq),
        m_context(batch),
        m_request(std::in_place_t(), RequestData{ url, std::move(requestData), std::move(requestMetadata) }) {

    }

    ExecutingCall::~ExecutingCall() = default;

    Il2CppObject *ExecutingCall::invoke(void *onObject, const MethodInfo *method, void **args) {
        Il2CppException *exception = nullptr;
        auto result = il2cpp_runtime_invoke(method, onObject, args, &exception);
        if(exception) {
            char message[256];
            il2cpp_format_exception(exception, message, sizeof(message));
            throw std::runtime_error(std::string(message, strnlen(message, sizeof(message))));
        }

        return result;
    }

    bool ExecutingCall::update() {
        if(!m_unityRequest) {
            m_unityRequest.emplace(m_request.value());
            m_request.reset();
        }

        if(!m_unityRequest->isDone()) {
            return false;
        }

        auto systemError = m_unityRequest->systemError();
        if(systemError.has_value()) {
            m_context->fail(std::move(*systemError));
            m_cq->post(m_context, 1);

            return true;
        }


        m_context->metadata().clear();

        auto headers = m_unityRequest->responseHeaders();
        for(auto &&header: headers) {
            m_context->metadata().add(std::move(header.first), std::move(header.second));
        }

        auto responseCode = m_unityRequest->responseCode();
        if(responseCode != 200) {
            auto contentType = m_unityRequest->responseHeader("Content-Type");
            if(contentType.has_value() && *contentType == "text/plain") {
                m_context->fail(m_unityRequest->responseText());
            } else {

                std::stringstream message;
                message << "HTTP error " << responseCode;
                m_context->fail(message.str());
            }

            m_cq->post(m_context, 1);

            return true;
        }

        m_context->replaceResponseData(m_unityRequest->responseData());
        m_cq->post(m_context, 1);

        return true;
    }

    ExecutingCall::UnityRequestSetup::UnityRequestSetup(const RequestData &requestData) {

        auto downloadHandlerObject = il2cpp_object_new(DownloadHandlerBufferClass);
        invoke(downloadHandlerObject, DownloadHandlerBuffer_ctor, nullptr);

        m_downloadHandler = GCHandle(downloadHandlerObject);

        Il2CppObject *uploadHandlerObject = nullptr;

        if(!requestData.requestData.empty()) {

            uploadHandlerObject = il2cpp_object_new(UploadHandlerRawClass);

            auto requestArray = il2cpp_array_new(
                SystemByteClass,
                requestData.requestData.size()
            );

            memcpy(reinterpret_cast<unsigned char *>(requestArray) + ArrayHeaderSize, requestData.requestData.data(),
                requestData.requestData.size());

            std::array<void *, 1> uploadHandlerArgs{
                requestArray
            };

            invoke(uploadHandlerObject, UploadHandlerRaw_ctor, uploadHandlerArgs.data());
        }

        auto webRequestObject = il2cpp_object_new(UnityWebRequestClass);

        auto urlString = il2cpp_string_new_len(requestData.url.c_str(), requestData.url.size());
        auto methodString = il2cpp_string_new("POST");

        std::array<void *, 4> args{
            urlString,
            methodString,
            downloadHandlerObject,
            uploadHandlerObject
        };

        invoke(webRequestObject, UnityWebRequest_ctor, args.data());

        m_webRequest.emplace(webRequestObject);

        setWebRequestHeader(webRequestObject, "Content-Type", "application/octet-stream");

        for(const auto &header: requestData.requestMetadata) {
            setWebRequestHeader(webRequestObject, header.first, header.second);
        }

        invoke(webRequestObject, UnityWebRequest_SendWebRequest, nullptr);
    }

    void ExecutingCall::setWebRequestHeader(Il2CppObject *webRequestObject, const std::string_view &keyString, const std::string_view &valueString) {

        std::array<void *, 2> args{
            stringFromUtf8(keyString),
            stringFromUtf8(valueString)
        };

        invoke(webRequestObject, UnityWebRequest_SetRequestHeader, args.data());
    }

    bool ExecutingCall::UnityRequestSetup::isDone() const {
        return *static_cast<bool *>(il2cpp_object_unbox(invoke(m_webRequest->get(), UnityWebRequest_isDone_get, nullptr)));
    }

    int32_t ExecutingCall::UnityRequestSetup::responseCode() const {
        return *static_cast<int32_t *>(il2cpp_object_unbox(invoke(m_webRequest->get(), UnityWebRequest_responseCode_get, nullptr)));
    }

    std::optional<std::string> ExecutingCall::UnityRequestSetup::systemError() const {
        auto netString = reinterpret_cast<Il2CppString *>(
            invoke(m_webRequest->get(), UnityWebRequest_error_get, nullptr)
        );

        if(!netString)
            return std::nullopt;

        return stringToUtf8(netString);
    }

    std::optional<std::string> ExecutingCall::UnityRequestSetup::responseHeader(const std::string_view &name) const {
        auto nameString = stringFromUtf8(name);

        std::array<void *, 1> args{ nameString };
        auto valueString = reinterpret_cast<Il2CppString *>(invoke(m_webRequest->get(), UnityWebRequest_GetResponseHeader, args.data()));
        if(!valueString)
            return std::nullopt;

        return stringToUtf8(valueString);
    }

    std::string ExecutingCall::UnityRequestSetup::responseText() const {
        auto text = reinterpret_cast<Il2CppString *>(
            invoke(m_downloadHandler.get(), DownloadHandlerBuffer_text_get, nullptr)
        );

        return stringToUtf8(text);
    }

    std::vector<unsigned char> ExecutingCall::UnityRequestSetup::responseData() const {
        auto data = reinterpret_cast<Il2CppArray *>(
            invoke(m_downloadHandler.get(), DownloadHandlerBuffer_data_get, nullptr)
        );

        ArrayWrapper<unsigned char> dataWrapper(data);

        return std::vector<unsigned char>(dataWrapper.data(), dataWrapper.data() + dataWrapper.size());
    }

    std::vector<Metadata> ExecutingCall::UnityRequestSetup::responseHeaders() const {
        /*
         * GetResponseHeaders is absent in the AOT'ed code, so we need to go
         * through the icall directly.
         */

        auto request = m_webRequest->get();

        ArrayWrapper<Il2CppString *> keys(UnityWebRequest_GetResponseHeaderKeys(request));

        std::vector<Metadata> output;
        output.reserve(keys.size());

        for(size_t index = 0; index < keys.size(); index++) {
            auto key = keys.data()[index];

            std::array<void *, 1> args{ key };
            auto value = reinterpret_cast<Il2CppString *>(invoke(request, UnityWebRequest_GetResponseHeader, args.data()));

            output.emplace_back(stringToUtf8(key), stringToUtf8(value));
        }

        return output;
    }

    ExecutingCall::UnityRequestSetup::~UnityRequestSetup() = default;

    ExecutingCall::WebRequestDisposer::WebRequestDisposer(Il2CppObject *object) : m_handle(object) {

    }

    ExecutingCall::WebRequestDisposer::~WebRequestDisposer() {
        auto object = m_handle.get();
        if(object) {
            invoke(object, UnityWebRequest_Dispose, nullptr);
        }
    }
}

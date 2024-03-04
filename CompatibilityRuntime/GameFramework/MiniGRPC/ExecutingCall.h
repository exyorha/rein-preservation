#ifndef EXECUTING_CALL_H
#define EXECUTING_CALL_H

#include <string>
#include <vector>
#include <optional>
#include <cstdint>
#include <il2cpp-api-types.h>

#include "GCHandle.h"
#include "MetadataArray.h"

namespace minigrpc {

    class CompletionQueue;
    class BatchContext;

    class ExecutingCall {
    public:
        ExecutingCall(
            const std::string &url,
            CompletionQueue *cq,
            BatchContext *batch,
            std::vector<unsigned char> &&requestData,
            std::vector<std::pair<std::string, std::string>> &&requestMetadata
        );
        ~ExecutingCall();

        ExecutingCall(const ExecutingCall &other) = delete;
        ExecutingCall &operator =(const ExecutingCall &other) = delete;

        /*
         * Returns true when this call has finished processing and can be removed.
         */
        bool update();

    private:
        struct RequestData {
            std::string url;
            std::vector<unsigned char> requestData;
            std::vector<std::pair<std::string, std::string>> requestMetadata;
        };

        class WebRequestDisposer {
        public:
            explicit WebRequestDisposer(Il2CppObject *object);
            ~WebRequestDisposer();

            WebRequestDisposer(const WebRequestDisposer &other) = delete;
            WebRequestDisposer &operator =(const WebRequestDisposer &other) = delete;

            inline Il2CppObject *get() const {
                return m_handle.get();
            }

        private:
            GCHandle m_handle;
        };

        class UnityRequestSetup {
        public:
            UnityRequestSetup(const RequestData &data);
            ~UnityRequestSetup();

            UnityRequestSetup(const UnityRequestSetup &other) = delete;
            UnityRequestSetup &operator =(const UnityRequestSetup &other) = delete;

            bool isDone() const;
            std::optional<std::string> systemError() const;

            int32_t responseCode() const;
            std::vector<Metadata> responseHeaders() const;
            std::optional<std::string> responseHeader(const std::string_view &name) const;

            std::string responseText() const;
            std::vector<unsigned char> responseData() const;

        private:
            GCHandle m_downloadHandler;
            std::optional<WebRequestDisposer> m_webRequest;
        };

        static Il2CppObject *invoke(void *onObject, const MethodInfo *method, void **args);

        static void setWebRequestHeader(Il2CppObject *object, const std::string_view &key, const std::string_view &value);

        CompletionQueue *m_cq;
        BatchContext *m_context;
        std::optional<RequestData> m_request;
        std::optional<UnityRequestSetup> m_unityRequest;
    };

}

#endif

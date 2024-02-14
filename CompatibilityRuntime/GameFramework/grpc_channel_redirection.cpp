#include "grpc_channel_redirection.h"
#include "grpc_csharp_ext.h"
#include <translator_api.h>

static TranslatorGrpcRedirectionCallback RedirectionCallback;

static grpc_channel *redirectAndCreateChannel(TranslatorGrpcChannelSetup *setup) {

    setup->channel = nullptr;

    if(RedirectionCallback) {
        RedirectionCallback(setup);
    }

    if(setup->channel) {
        return static_cast<grpc_channel *>(setup->channel);

    } else if(setup->secure) {
        return grpcsharp_secure_channel_create(
            static_cast<grpc_channel_credentials *>(setup->creds),
            setup->target,
            static_cast<const grpc_channel_args *>(setup->args));
    } else {
        return grpcsharp_insecure_channel_create(setup->target, static_cast<const grpc_channel_args *>(setup->args));
    }
}

grpc_channel* GPR_CALLTYPE grpcsharp_secure_channel_create_with_redirection(
    grpc_channel_credentials* creds, const char* target,
    const grpc_channel_args* args) {

    TranslatorGrpcChannelSetup setup;
    setup.creds = creds;
    setup.target = target;
    setup.args = args;
    setup.secure = 1;
    return redirectAndCreateChannel(&setup);
}

grpc_channel* GPR_CALLTYPE
grpcsharp_insecure_channel_create_with_redirection(const char* target, const grpc_channel_args* args) {

    TranslatorGrpcChannelSetup setup;
    setup.creds = nullptr;
    setup.target = target;
    setup.args = args;
    setup.secure = 0;
    return redirectAndCreateChannel(&setup);
}

void translator_set_grpc_redirection_callback(TranslatorGrpcRedirectionCallback callback) {
    RedirectionCallback = callback;
}


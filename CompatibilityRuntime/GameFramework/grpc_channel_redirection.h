#ifndef GRPC_CHANNEL_DIRECTION_H
#define GRPC_CHANNEL_DIRECTION_H

#include "grpc_csharp_ext.h"

GPR_EXPORT grpc_channel* GPR_CALLTYPE grpcsharp_secure_channel_create_with_redirection(
    grpc_channel_credentials* creds, const char* target,
    const grpc_channel_args* args);

GPR_EXPORT grpc_channel* GPR_CALLTYPE
grpcsharp_insecure_channel_create_with_redirection(const char* target,
                                  const grpc_channel_args* args);


#endif

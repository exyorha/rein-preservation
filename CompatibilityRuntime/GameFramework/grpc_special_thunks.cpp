#include "grpc_special_thunks.h"
#include "grpc_csharp_ext.h"

#include <Translator/JITThreadContext.h>
#include <Translator/thunking.h>

// GPR_EXPORT gpr_timespec GPR_CALLTYPE
// gprsharp_convert_clock_type(gpr_timespec t, gpr_clock_type target_clock) ;
void gprsharp_convert_clock_type_raw_thunk(void) {
    auto &context = JITThreadContext::get();

    gpr_timespec input;
    memcpy(&input, &context.gprs[0], sizeof(input));

    gpr_clock_type input_clock = static_cast<gpr_clock_type>(context.gprs[2]);

    storeX86CallResult(gprsharp_convert_clock_type(input, input_clock));
}

//GPR_EXPORT void GPR_CALLTYPE grpcsharp_channel_watch_connectivity_state(
//    grpc_channel* channel, grpc_connectivity_state last_observed_state,
//    gpr_timespec deadline, grpc_completion_queue* cq,
//    grpcsharp_batch_context* ctx);
void grpcsharp_channel_watch_connectivity_state_raw_thunk(void) {

    auto &context = JITThreadContext::get();

    auto channel = retrieveX86CallArgument<grpc_channel *>(0);
    auto last_observed_state = retrieveX86CallArgument<grpc_connectivity_state>(1);

    gpr_timespec deadline;
    memcpy(&deadline, &context.gprs[2], sizeof(deadline));

    auto cq = retrieveX86CallArgument<grpc_completion_queue *>(4);
    auto ctx = retrieveX86CallArgument<grpcsharp_batch_context *>(5);

    return grpcsharp_channel_watch_connectivity_state(channel, last_observed_state, deadline, cq, ctx);
}

//GPR_EXPORT grpc_call* GPR_CALLTYPE grpcsharp_channel_create_call(
//    grpc_channel* channel, grpc_call* parent_call, uint32_t propagation_mask,
//    grpc_completion_queue* cq, const char* method, const char* host,
//    gpr_timespec deadline);
void grpcsharp_channel_create_call_raw_thunk(void) {
    auto &context = JITThreadContext::get();

    auto channel = retrieveX86CallArgument<grpc_channel *>(0);
    auto parent_call = retrieveX86CallArgument<grpc_call *>(1);
    auto propagation_mask = retrieveX86CallArgument<uint32_t>(2);
    auto cq = retrieveX86CallArgument<grpc_completion_queue *>(3);
    auto method = retrieveX86CallArgument<const char *>(4);
    auto host = retrieveX86CallArgument<const char *>(5);

    gpr_timespec deadline;
    memcpy(&deadline, &context.gprs[6], sizeof(deadline));

    storeX86CallResult(grpcsharp_channel_create_call(channel, parent_call, propagation_mask, cq, method, host, deadline));
}


//GPR_EXPORT grpc_auth_property_iterator GPR_CALLTYPE
//  grpcsharp_auth_context_property_iterator(const grpc_auth_context* ctx);
//  'grpc_auth_property_iterator' is large and is returned as a pointer in x8
void grpcsharp_auth_context_property_iterator_raw_thunk(void) {
    auto &context = JITThreadContext::get();

    auto ctx = retrieveX86CallArgument<const grpc_auth_context *>(0);

    auto pointerToInterator = reinterpret_cast<grpc_auth_property_iterator *>(context.gprs[8]);
    *pointerToInterator  = grpcsharp_auth_context_property_iterator(ctx);
}



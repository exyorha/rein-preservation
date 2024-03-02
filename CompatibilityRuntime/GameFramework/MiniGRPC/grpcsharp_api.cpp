#include "grpc_csharp_ext.h"
#include "grpcsharp.h"
#include "CompletionQueue.h"
#include "ChannelArgs.h"
#include "Channel.h"
#include "Call.h"
#include "SliceBuffer.h"
#include "MetadataArray.h"
#include "BatchContext.h"

#include <stdexcept>

/*
 * Entirely dummy objects, only exists so we can pass non-null pointers of them around.
 */
struct grpc_channel_credentials {
   unsigned char dummy;
};

struct grpc_call_credentials {
    unsigned char dummy;
};

namespace minigrpc {
    grpcsharp_log_func logFunction = nullptr;
    grpcsharp_native_callback_dispatcher_func nativeCallbackDispatcher = nullptr;
}

void GPR_CALLTYPE grpcsharp_metadata_array_destroy_full(grpc_metadata_array* array) {
    delete reinterpret_cast<minigrpc::MetadataArray *>(array);
}

grpc_metadata_array* GPR_CALLTYPE grpcsharp_metadata_array_create(size_t capacity) {
    return reinterpret_cast<grpc_metadata_array *>(new minigrpc::MetadataArray(capacity));
}

void GPR_CALLTYPE
grpcsharp_metadata_array_add(grpc_metadata_array* array, const char* key,
                             const char* value, size_t value_length) {

    reinterpret_cast<minigrpc::MetadataArray *>(array)->add(key, std::string(value, value + value_length));
}

intptr_t GPR_CALLTYPE
grpcsharp_metadata_array_count(grpc_metadata_array* array) {

    return reinterpret_cast<minigrpc::MetadataArray *>(array)->metadata().size();
}

const char* GPR_CALLTYPE grpcsharp_metadata_array_get_key(
    grpc_metadata_array* array, size_t index, size_t* key_length) {

    const auto &item = reinterpret_cast<minigrpc::MetadataArray *>(array)->metadata().at(index);
    *key_length = item.first.size();
    return item.first.data();
}

const char* GPR_CALLTYPE grpcsharp_metadata_array_get_value(
    grpc_metadata_array* array, size_t index, size_t* value_length) {

    const auto &item = reinterpret_cast<minigrpc::MetadataArray *>(array)->metadata().at(index);
    *value_length = item.second.size();
    return item.second.data();
}

grpcsharp_batch_context* GPR_CALLTYPE grpcsharp_batch_context_create(void) {
    return reinterpret_cast<grpcsharp_batch_context *>(new minigrpc::BatchContext);
}

void GPR_CALLTYPE
grpcsharp_batch_context_reset(grpcsharp_batch_context* ctx) {

    reinterpret_cast<minigrpc::BatchContext *>(ctx)->reset();
}

void GPR_CALLTYPE
grpcsharp_batch_context_destroy(grpcsharp_batch_context* ctx) {

    delete reinterpret_cast<minigrpc::BatchContext *>(ctx);
}

const grpc_metadata_array* GPR_CALLTYPE
grpcsharp_batch_context_recv_initial_metadata(
    const grpcsharp_batch_context* ctx) {

    auto bctx = reinterpret_cast<const minigrpc::BatchContext *>(ctx);
    return reinterpret_cast<const grpc_metadata_array *>(&bctx->metadata());
}

intptr_t GPR_CALLTYPE grpcsharp_batch_context_recv_message_length(
    const grpcsharp_batch_context* ctx) {

    auto bctx = reinterpret_cast<const minigrpc::BatchContext *>(ctx);
    return bctx->responseSize();
}

int GPR_CALLTYPE
grpcsharp_batch_context_recv_message_next_slice_peek(
    grpcsharp_batch_context* ctx, size_t* slice_len, uint8_t** slice_data_ptr) {

    auto bctx = reinterpret_cast< minigrpc::BatchContext *>(ctx);

    auto slice = bctx->responseData();
    if(!slice)
        return 0;

    *slice_len = slice->size();
    *slice_data_ptr = slice->data();

    return 1;
}

grpc_status_code GPR_CALLTYPE
grpcsharp_batch_context_recv_status_on_client_status(
    const grpcsharp_batch_context* ctx) {

    if(reinterpret_cast<const minigrpc::BatchContext *>(ctx)->failed()) {
        printf("reporting an error\n");
        return 2; /* unknown error */
    } else
        return 0;
}

const char* GPR_CALLTYPE
grpcsharp_batch_context_recv_status_on_client_details(
    const grpcsharp_batch_context* ctx, size_t* details_length) {

    auto bctx = reinterpret_cast<const minigrpc::BatchContext *>(ctx);
    if(bctx->failed()) {

        const auto &error = bctx->error();

        *details_length = error.size();
        return error.data();
    } else {
        *details_length = 0;
        return "";
    }
}

const char* GPR_CALLTYPE
grpcsharp_batch_context_recv_status_on_client_error_string(
    const grpcsharp_batch_context* ctx) {

    size_t len;

    return grpcsharp_batch_context_recv_status_on_client_details(ctx, &len);
}

const grpc_metadata_array* GPR_CALLTYPE
grpcsharp_batch_context_recv_status_on_client_trailing_metadata(
    const grpcsharp_batch_context* ctx) {

    auto bctx = reinterpret_cast<const minigrpc::BatchContext *>(ctx);
    return reinterpret_cast<const grpc_metadata_array *>(&bctx->metadata());
}

int32_t GPR_CALLTYPE
grpcsharp_batch_context_recv_close_on_server_cancelled(
    const grpcsharp_batch_context* ctx) {
    throw std::runtime_error("grpcsharp_batch_context_recv_close_on_server_cancelled is not implemented in mini-gRPC");
}

grpcsharp_request_call_context* GPR_CALLTYPE grpcsharp_request_call_context_create(void) {
    throw std::runtime_error("grpcsharp_request_call_context_create is not implemented in mini-gRPC");
}

void GPR_CALLTYPE
grpcsharp_request_call_context_reset(grpcsharp_request_call_context* ctx) {
    throw std::runtime_error("grpcsharp_request_call_context_reset is not implemented in mini-gRPC");
}

void GPR_CALLTYPE
grpcsharp_request_call_context_destroy(grpcsharp_request_call_context* ctx) {
    throw std::runtime_error("grpcsharp_request_call_context_destroy is not implemented in mini-gRPC");
}

grpc_call* GPR_CALLTYPE
grpcsharp_request_call_context_call(const grpcsharp_request_call_context* ctx) {
    throw std::runtime_error("grpcsharp_request_call_context_call is not implemented in mini-gRPC");
}

const char* GPR_CALLTYPE grpcsharp_request_call_context_method(
    const grpcsharp_request_call_context* ctx, size_t* method_length) {
    throw std::runtime_error("grpcsharp_request_call_context_method is not implemented in mini-gRPC");
}

const char* GPR_CALLTYPE grpcsharp_request_call_context_host(
    const grpcsharp_request_call_context* ctx, size_t* host_length) {
    throw std::runtime_error("grpcsharp_request_call_context_host is not implemented in mini-gRPC");
}

gpr_timespec GPR_CALLTYPE grpcsharp_request_call_context_deadline(
    const grpcsharp_request_call_context* ctx) {
    throw std::runtime_error("grpcsharp_request_call_context_deadline is not implemented in mini-gRPC");
}

const grpc_metadata_array* GPR_CALLTYPE
grpcsharp_request_call_context_request_metadata(
    const grpcsharp_request_call_context* ctx) {
    throw std::runtime_error("grpcsharp_request_call_context_request_metadata is not implemented in mini-gRPC");
}

void GPR_CALLTYPE grpcsharp_init(void) {
    minigrpc::platformInitialize();
}

void GPR_CALLTYPE grpcsharp_shutdown(void) {
    minigrpc::platformFinalize();
}

grpc_completion_queue* GPR_CALLTYPE
grpcsharp_completion_queue_create_async(void) {

    return reinterpret_cast<grpc_completion_queue *>(new minigrpc::CompletionQueue(true));
}

grpc_completion_queue* GPR_CALLTYPE
grpcsharp_completion_queue_create_sync(void) {

    return reinterpret_cast<grpc_completion_queue *>(new minigrpc::CompletionQueue(false));
}

void GPR_CALLTYPE
grpcsharp_completion_queue_shutdown(grpc_completion_queue* cq) {

    return reinterpret_cast<minigrpc::CompletionQueue *>(cq)->shutdown();
}

void GPR_CALLTYPE
grpcsharp_completion_queue_destroy(grpc_completion_queue* cq) {

    delete reinterpret_cast<minigrpc::CompletionQueue *>(cq);
}

grpc_event GPR_CALLTYPE
grpcsharp_completion_queue_next(grpc_completion_queue* cq) {

    return reinterpret_cast<minigrpc::CompletionQueue *>(cq)->next();
}

grpc_event GPR_CALLTYPE
grpcsharp_completion_queue_pluck(grpc_completion_queue* cq, void* tag) {

    throw std::runtime_error("grpcsharp_completion_queue_pluck is not implemented in mini-gRPC");
}

grpc_channel* GPR_CALLTYPE
grpcsharp_insecure_channel_create(const char* target,
                                  const grpc_channel_args* args) {

    return reinterpret_cast<grpc_channel *>(new minigrpc::Channel(target, reinterpret_cast<const minigrpc::ChannelArgs *>(args)));
}

void GPR_CALLTYPE grpcsharp_channel_destroy(grpc_channel* channel) {

    delete reinterpret_cast<minigrpc::Channel *>(channel);
}

grpc_connectivity_state GPR_CALLTYPE
grpcsharp_channel_check_connectivity_state(grpc_channel* channel,
                                           int32_t try_to_connect) {
    throw std::runtime_error("grpcsharp_channel_check_connectivity_state is not implemented in mini-gRPC");
}

void GPR_CALLTYPE grpcsharp_channel_watch_connectivity_state(
    grpc_channel* channel, grpc_connectivity_state last_observed_state,
    gpr_timespec deadline, grpc_completion_queue* cq,
    grpcsharp_batch_context* ctx) {
    throw std::runtime_error("grpcsharp_channel_watch_connectivity_state is not implemented in mini-gRPC");
}

char* GPR_CALLTYPE
grpcsharp_channel_get_target(grpc_channel* channel) {

    return const_cast<char *>(reinterpret_cast<minigrpc::Channel *>(channel)->target().c_str());
}

grpc_channel_args* GPR_CALLTYPE
grpcsharp_channel_args_create(size_t num_args) {

    return reinterpret_cast<grpc_channel_args *>(new minigrpc::ChannelArgs(num_args));
}

void GPR_CALLTYPE grpcsharp_channel_args_set_string(
    grpc_channel_args* args, size_t index, const char* key, const char* value) {

    reinterpret_cast<minigrpc::ChannelArgs *>(args)->setString(index, key, value);
}

void GPR_CALLTYPE grpcsharp_channel_args_set_integer(
    grpc_channel_args* args, size_t index, const char* key, int value) {

    reinterpret_cast<minigrpc::ChannelArgs *>(args)->setInteger(index, key, value);
}

void GPR_CALLTYPE
grpcsharp_channel_args_destroy(grpc_channel_args* args) {

    delete reinterpret_cast<minigrpc::ChannelArgs *>(args);
}

grpc_call* GPR_CALLTYPE grpcsharp_channel_create_call(
    grpc_channel* channel, grpc_call* parent_call, uint32_t propagation_mask,
    grpc_completion_queue* cq, const char* method, const char* host,
    gpr_timespec deadline) {

    (void)parent_call;
    (void)propagation_mask;
    (void)host;
    (void)deadline;

    return reinterpret_cast<grpc_call *>(
        new minigrpc::Call(
            reinterpret_cast<const minigrpc::Channel *>(channel),
            reinterpret_cast<minigrpc::CompletionQueue *>(cq),
            method));
}

grpc_call_error GPR_CALLTYPE grpcsharp_call_cancel(grpc_call* call) {
    throw std::runtime_error("grpcsharp_call_cancel is not implemented in mini-gRPC");
}

grpc_call_error GPR_CALLTYPE grpcsharp_call_cancel_with_status(
    grpc_call* call, grpc_status_code status, const char* description) {
    throw std::runtime_error("grpcsharp_call_cancel_with_status is not implemented in mini-gRPC");
}

char* GPR_CALLTYPE grpcsharp_call_get_peer(grpc_call* call) {
    throw std::runtime_error("grpcsharp_call_get_peer is not implemented in mini-gRPC");
}

void GPR_CALLTYPE grpcsharp_call_destroy(grpc_call* call) {
    delete reinterpret_cast<minigrpc::Call *>(call);
}

grpc_call_error GPR_CALLTYPE grpcsharp_call_start_unary(
    grpc_call* call, grpcsharp_batch_context* ctx,
    grpc_slice_buffer* send_buffer, uint32_t write_flags,
    grpc_metadata_array* initial_metadata, uint32_t initial_metadata_flags) {

    reinterpret_cast<minigrpc::Call *>(call)->start(
        reinterpret_cast<minigrpc::BatchContext *>(ctx),
        reinterpret_cast<minigrpc::SliceBuffer *>(send_buffer),
        reinterpret_cast<minigrpc::MetadataArray *>(initial_metadata)
    );

    return 0;
}

grpc_call_error GPR_CALLTYPE grpcsharp_test_call_start_unary_echo(
    grpc_call* call, grpcsharp_batch_context* ctx,
    grpc_slice_buffer* send_buffer, uint32_t write_flags,
    grpc_metadata_array* initial_metadata, uint32_t initial_metadata_flags) {
    throw std::runtime_error("grpcsharp_test_call_start_unary_echo is not implemented in mini-gRPC");
}

grpc_call_error GPR_CALLTYPE grpcsharp_call_start_client_streaming(
    grpc_call* call, grpcsharp_batch_context* ctx,
    grpc_metadata_array* initial_metadata, uint32_t initial_metadata_flags) {
    throw std::runtime_error("grpcsharp_call_start_client_streaming is not implemented in mini-gRPC");
}

grpc_call_error GPR_CALLTYPE grpcsharp_call_start_server_streaming(
    grpc_call* call, grpcsharp_batch_context* ctx,
    grpc_slice_buffer* send_buffer, uint32_t write_flags,
    grpc_metadata_array* initial_metadata, uint32_t initial_metadata_flags) {
    throw std::runtime_error("grpcsharp_call_start_server_streaming is not implemented in mini-gRPC");
}

grpc_call_error GPR_CALLTYPE grpcsharp_call_start_duplex_streaming(
    grpc_call* call, grpcsharp_batch_context* ctx,
    grpc_metadata_array* initial_metadata, uint32_t initial_metadata_flags) {
    throw std::runtime_error("grpcsharp_call_start_duplex_streaming is not implemented in mini-gRPC");
}

grpc_call_error GPR_CALLTYPE grpcsharp_call_recv_initial_metadata(
    grpc_call* call, grpcsharp_batch_context* ctx) {
    throw std::runtime_error("grpcsharp_call_recv_initial_metadata is not implemented in mini-gRPC");
}

grpc_call_error GPR_CALLTYPE grpcsharp_call_send_message(
    grpc_call* call, grpcsharp_batch_context* ctx,
    grpc_slice_buffer* send_buffer, uint32_t write_flags,
    int32_t send_empty_initial_metadata) {
    throw std::runtime_error("grpcsharp_call_send_message is not implemented in mini-gRPC");
}

grpc_call_error GPR_CALLTYPE grpcsharp_call_send_close_from_client(
    grpc_call* call, grpcsharp_batch_context* ctx) {
    throw std::runtime_error("grpcsharp_call_send_close_from_client is not implemented in mini-gRPC");
}

grpc_call_error GPR_CALLTYPE grpcsharp_call_send_status_from_server(
    grpc_call* call, grpcsharp_batch_context* ctx, grpc_status_code status_code,
    const char* status_details, size_t status_details_len,
    grpc_metadata_array* trailing_metadata, int32_t send_empty_initial_metadata,
    grpc_slice_buffer* optional_send_buffer, uint32_t write_flags) {
    throw std::runtime_error("grpcsharp_call_send_status_from_server is not implemented in mini-gRPC");
}

grpc_call_error GPR_CALLTYPE
grpcsharp_call_recv_message(grpc_call* call, grpcsharp_batch_context* ctx) {
    throw std::runtime_error("grpcsharp_call_recv_message is not implemented in mini-gRPC");
}

grpc_call_error GPR_CALLTYPE
grpcsharp_call_start_serverside(grpc_call* call, grpcsharp_batch_context* ctx) {
    throw std::runtime_error("grpcsharp_call_start_serverside is not implemented in mini-gRPC");
}

grpc_call_error GPR_CALLTYPE grpcsharp_call_send_initial_metadata(
    grpc_call* call, grpcsharp_batch_context* ctx,
    grpc_metadata_array* initial_metadata) {
    throw std::runtime_error("grpcsharp_call_send_initial_metadata is not implemented in mini-gRPC");
}

grpc_call_error GPR_CALLTYPE
grpcsharp_call_set_credentials(grpc_call* call, grpc_call_credentials* creds) {

    return 0;
}

gpr_timespec GPR_CALLTYPE gprsharp_now(gpr_clock_type clock_type) {
    throw std::runtime_error("gprsharp_now is not implemented in mini-gRPC");
}

gpr_timespec GPR_CALLTYPE
gprsharp_inf_future(gpr_clock_type clock_type) {
    throw std::runtime_error("gprsharp_inf_future is not implemented in mini-gRPC");
}

gpr_timespec GPR_CALLTYPE
gprsharp_inf_past(gpr_clock_type clock_type) {
    throw std::runtime_error("gprsharp_inf_past is not implemented in mini-gRPC");
}

gpr_timespec GPR_CALLTYPE
gprsharp_convert_clock_type(gpr_timespec t, gpr_clock_type target_clock)  {
    throw std::runtime_error("gprsharp_convert_clock_type is not implemented in mini-gRPC");
}

int32_t GPR_CALLTYPE gprsharp_sizeof_timespec(void) {

    return sizeof(gpr_timespec);
}

void GPR_CALLTYPE gprsharp_free(void* p) {
    throw std::runtime_error("gprsharp_free is not implemented in mini-gRPC");
}

grpc_server* GPR_CALLTYPE
grpcsharp_server_create(const grpc_channel_args* args) {
    throw std::runtime_error("grpcsharp_server_create is not implemented in mini-gRPC");
}

void GPR_CALLTYPE grpcsharp_server_register_completion_queue(
    grpc_server* server, grpc_completion_queue* cq) {
    throw std::runtime_error("grpcsharp_server_register_completion_queue is not implemented in mini-gRPC");
}

int32_t GPR_CALLTYPE grpcsharp_server_add_insecure_http2_port(
    grpc_server* server, const char* addr) {
    throw std::runtime_error("grpcsharp_server_add_insecure_http2_port is not implemented in mini-gRPC");
}

int32_t GPR_CALLTYPE grpcsharp_server_add_secure_http2_port(
    grpc_server* server, const char* addr, grpc_server_credentials* creds) {
    throw std::runtime_error("grpcsharp_server_add_secure_http2_port is not implemented in mini-gRPC");
}

void GPR_CALLTYPE grpcsharp_server_start(grpc_server* server) {
    throw std::runtime_error("grpcsharp_server_start is not implemented in mini-gRPC");
}

void GPR_CALLTYPE grpcsharp_server_shutdown_and_notify_callback(
    grpc_server* server, grpc_completion_queue* cq,
    grpcsharp_batch_context* ctx) {
    throw std::runtime_error("grpcsharp_server_shutdown_and_notify_callback is not implemented in mini-gRPC");
}

void GPR_CALLTYPE
grpcsharp_server_cancel_all_calls(grpc_server* server) {
    throw std::runtime_error("grpcsharp_server_cancel_all_calls is not implemented in mini-gRPC");
}

void GPR_CALLTYPE grpcsharp_server_destroy(grpc_server* server) {
    throw std::runtime_error("grpcsharp_server_destroy is not implemented in mini-gRPC");
}

grpc_call_error GPR_CALLTYPE
grpcsharp_server_request_call(grpc_server* server, grpc_completion_queue* cq,
                              grpcsharp_request_call_context* ctx) {
    throw std::runtime_error("grpcsharp_server_request_call is not implemented in mini-gRPC");
}

void GPR_CALLTYPE grpcsharp_native_callback_dispatcher_init(
    grpcsharp_native_callback_dispatcher_func func) {

    minigrpc::nativeCallbackDispatcher = func;
}


void GPR_CALLTYPE
grpcsharp_override_default_ssl_roots(const char* pem_root_certs) {
    (void)pem_root_certs;
}

grpc_channel_credentials* GPR_CALLTYPE
grpcsharp_ssl_credentials_create(const char* pem_root_certs,
                                 const char* key_cert_pair_cert_chain,
                                 const char* key_cert_pair_private_key,
                                 void* verify_peer_callback_tag) {

    return new grpc_channel_credentials;
}

void GPR_CALLTYPE
grpcsharp_channel_credentials_release(grpc_channel_credentials* creds) {

    delete creds;
}


void GPR_CALLTYPE
grpcsharp_call_credentials_release(grpc_call_credentials* creds) {

    delete creds;
}

grpc_channel* GPR_CALLTYPE grpcsharp_secure_channel_create(
    grpc_channel_credentials* creds, const char* target,
    const grpc_channel_args* args) {

    return reinterpret_cast<grpc_channel *>(new minigrpc::Channel(target, reinterpret_cast<const minigrpc::ChannelArgs *>(args)));
}

grpc_server_credentials* GPR_CALLTYPE
grpcsharp_ssl_server_credentials_create(
    const char* pem_root_certs, const char** key_cert_pair_cert_chain_array,
    const char** key_cert_pair_private_key_array, size_t num_key_cert_pairs,
    grpc_ssl_client_certificate_request_type client_request_type) {
    throw std::runtime_error("grpcsharp_ssl_server_credentials_create is not implemented in mini-gRPC");
}

void GPR_CALLTYPE
grpcsharp_server_credentials_release(grpc_server_credentials* creds) {
    throw std::runtime_error("grpcsharp_server_credentials_release is not implemented in mini-gRPC");
}

grpc_channel_credentials* GPR_CALLTYPE
grpcsharp_composite_channel_credentials_create(
    grpc_channel_credentials* channel_creds,
    grpc_call_credentials* call_creds) {

    return new grpc_channel_credentials;
}

grpc_call_credentials* GPR_CALLTYPE
grpcsharp_composite_call_credentials_create(grpc_call_credentials* creds1,
                                            grpc_call_credentials* creds2) {

    return new grpc_call_credentials;
}

void GPR_CALLTYPE grpcsharp_metadata_credentials_notify_from_plugin(
    grpc_credentials_plugin_metadata_cb cb, void* user_data,
    grpc_metadata_array* metadata, grpc_status_code status,
    const char* error_details) {
    throw std::runtime_error("grpcsharp_metadata_credentials_notify_from_plugin is not implemented in mini-gRPC");
}


grpc_call_credentials* GPR_CALLTYPE
grpcsharp_metadata_credentials_create_from_plugin(void* callback_tag) {
    throw std::runtime_error("grpcsharp_metadata_credentials_create_from_plugin is not implemented in mini-gRPC");
}

grpc_auth_context* GPR_CALLTYPE
grpcsharp_call_auth_context(grpc_call* call) {
    throw std::runtime_error("grpcsharp_metadata_array_create is not implemented in mini-gRPC");
}

const char* GPR_CALLTYPE
grpcsharp_auth_context_peer_identity_property_name(
    const grpc_auth_context* ctx) {
    throw std::runtime_error("grpcsharp_auth_context_peer_identity_property_name is not implemented in mini-gRPC");
}

grpc_auth_property_iterator GPR_CALLTYPE
grpcsharp_auth_context_property_iterator(const grpc_auth_context* ctx) {
    throw std::runtime_error("grpcsharp_auth_context_property_iterator is not implemented in mini-gRPC");
}

const grpc_auth_property* GPR_CALLTYPE
grpcsharp_auth_property_iterator_next(grpc_auth_property_iterator* it) {
    throw std::runtime_error("grpcsharp_auth_property_iterator_next is not implemented in mini-gRPC");
}

void GPR_CALLTYPE
grpcsharp_auth_context_release(grpc_auth_context* ctx) {
    throw std::runtime_error("grpcsharp_auth_context_release is not implemented in mini-gRPC");
}

void GPR_CALLTYPE grpcsharp_redirect_log(grpcsharp_log_func func) {
    minigrpc::logFunction = func;
}

grpc_slice_buffer* GPR_CALLTYPE grpcsharp_slice_buffer_create(void) {

    return reinterpret_cast<grpc_slice_buffer *>(new minigrpc::SliceBuffer);
}

void GPR_CALLTYPE
grpcsharp_slice_buffer_reset_and_unref(grpc_slice_buffer* buffer) {

    return reinterpret_cast<minigrpc::SliceBuffer *>(buffer)->reset();
}

void GPR_CALLTYPE
grpcsharp_slice_buffer_destroy(grpc_slice_buffer* buffer) {

    delete reinterpret_cast<minigrpc::SliceBuffer *>(buffer);
}

size_t GPR_CALLTYPE
grpcsharp_slice_buffer_slice_count(grpc_slice_buffer* inBuffer) {
    auto buffer = reinterpret_cast<minigrpc::SliceBuffer *>(inBuffer);

    if(buffer->empty())
        return 0;

    return 1;
}

void GPR_CALLTYPE
grpcsharp_slice_buffer_slice_peek(grpc_slice_buffer* inBuffer, size_t index,
                                  size_t* slice_len, uint8_t** slice_data_ptr) {

    auto buffer = reinterpret_cast<minigrpc::SliceBuffer *>(inBuffer);

    if(buffer->empty() || index != 0)
        throw std::logic_error("bad slice index");

    *slice_len = buffer->size();
    *slice_data_ptr = buffer->data();
}

void* GPR_CALLTYPE grpcsharp_slice_buffer_adjust_tail_space(
    grpc_slice_buffer* buffer, size_t available_tail_space,
    size_t requested_tail_space) {

    return reinterpret_cast<minigrpc::SliceBuffer *>(buffer)->adjustTailSpace(available_tail_space, requested_tail_space);
}

const char* GPR_CALLTYPE grpcsharp_version_string() {
    return "Mini-gRPC";
}

void GPR_CALLTYPE
grpcsharp_test_callback(test_callback_funcptr callback) {
    throw std::runtime_error("grpcsharp_test_callback is not implemented in mini-gRPC");
}

void* GPR_CALLTYPE grpcsharp_test_nop(void* ptr) {
    throw std::runtime_error("grpcsharp_test_nop is not implemented in mini-gRPC");
}

int32_t GPR_CALLTYPE grpcsharp_sizeof_grpc_event(void) {
    return sizeof(grpc_event);
}

void GPR_CALLTYPE
grpcsharp_test_override_method(const char* method_name, const char* variant) {
    throw std::runtime_error("grpcsharp_test_override_method is not implemented in mini-gRPC");
}

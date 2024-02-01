#ifndef GRPC_CSHARP_EXT_H
#define GRPC_CSHARP_EXT_H

#include <grpc/support/port_platform.h>

#include <grpc/grpc.h>
#include <grpc/grpc_security.h>

struct grpcsharp_batch_context;

typedef struct grpcsharp_batch_context grpcsharp_batch_context;
typedef struct grpcsharp_request_call_context grpcsharp_request_call_context;

#if 0
#ifdef GPR_WINDOWS
#define GPR_EXPORT __declspec(dllexport)
#define GPR_CALLTYPE __stdcall
#endif
#endif

#ifndef GPR_EXPORT
#define GPR_EXPORT
#endif

#ifndef GPR_CALLTYPE
#define GPR_CALLTYPE
#endif

typedef int(GPR_CALLTYPE* grpcsharp_native_callback_dispatcher_func)(
    void* tag, void* arg0, void* arg1, void* arg2, void* arg3, void* arg4,
    void* arg5);

typedef void(GPR_CALLTYPE* grpcsharp_log_func)(const char* file, int32_t line,
                                               uint64_t thd_id,
                                               const char* severity_string,
                                               const char* msg);

typedef void(GPR_CALLTYPE* test_callback_funcptr)(int32_t success);

#if defined(__cplusplus)
extern "C" {
#endif

GPR_EXPORT void GPR_CALLTYPE grpcsharp_metadata_array_destroy_full(grpc_metadata_array* array);

GPR_EXPORT grpc_metadata_array* GPR_CALLTYPE grpcsharp_metadata_array_create(size_t capacity);

GPR_EXPORT void GPR_CALLTYPE
grpcsharp_metadata_array_add(grpc_metadata_array* array, const char* key,
                             const char* value, size_t value_length);

GPR_EXPORT intptr_t GPR_CALLTYPE
grpcsharp_metadata_array_count(grpc_metadata_array* array);

GPR_EXPORT const char* GPR_CALLTYPE grpcsharp_metadata_array_get_key(
    grpc_metadata_array* array, size_t index, size_t* key_length);

GPR_EXPORT const char* GPR_CALLTYPE grpcsharp_metadata_array_get_value(
    grpc_metadata_array* array, size_t index, size_t* value_length);

GPR_EXPORT grpcsharp_batch_context* GPR_CALLTYPE grpcsharp_batch_context_create(void);

GPR_EXPORT void GPR_CALLTYPE
grpcsharp_batch_context_reset(grpcsharp_batch_context* ctx);

GPR_EXPORT void GPR_CALLTYPE
grpcsharp_batch_context_destroy(grpcsharp_batch_context* ctx);

GPR_EXPORT const grpc_metadata_array* GPR_CALLTYPE
grpcsharp_batch_context_recv_initial_metadata(
    const grpcsharp_batch_context* ctx);

GPR_EXPORT intptr_t GPR_CALLTYPE grpcsharp_batch_context_recv_message_length(
    const grpcsharp_batch_context* ctx);

GPR_EXPORT int GPR_CALLTYPE
grpcsharp_batch_context_recv_message_next_slice_peek(
    grpcsharp_batch_context* ctx, size_t* slice_len, uint8_t** slice_data_ptr);

GPR_EXPORT grpc_status_code GPR_CALLTYPE
grpcsharp_batch_context_recv_status_on_client_status(
    const grpcsharp_batch_context* ctx);


GPR_EXPORT const char* GPR_CALLTYPE
grpcsharp_batch_context_recv_status_on_client_details(
    const grpcsharp_batch_context* ctx, size_t* details_length);

GPR_EXPORT const char* GPR_CALLTYPE
grpcsharp_batch_context_recv_status_on_client_error_string(
    const grpcsharp_batch_context* ctx);

GPR_EXPORT const grpc_metadata_array* GPR_CALLTYPE
grpcsharp_batch_context_recv_status_on_client_trailing_metadata(
    const grpcsharp_batch_context* ctx);

GPR_EXPORT int32_t GPR_CALLTYPE
grpcsharp_batch_context_recv_close_on_server_cancelled(
    const grpcsharp_batch_context* ctx);

GPR_EXPORT grpcsharp_request_call_context* GPR_CALLTYPE grpcsharp_request_call_context_create(void);

GPR_EXPORT void GPR_CALLTYPE
grpcsharp_request_call_context_reset(grpcsharp_request_call_context* ctx);

GPR_EXPORT void GPR_CALLTYPE
grpcsharp_request_call_context_destroy(grpcsharp_request_call_context* ctx);

GPR_EXPORT grpc_call* GPR_CALLTYPE
grpcsharp_request_call_context_call(const grpcsharp_request_call_context* ctx);

GPR_EXPORT const char* GPR_CALLTYPE grpcsharp_request_call_context_method(
    const grpcsharp_request_call_context* ctx, size_t* method_length);

GPR_EXPORT const char* GPR_CALLTYPE grpcsharp_request_call_context_host(
    const grpcsharp_request_call_context* ctx, size_t* host_length);

GPR_EXPORT gpr_timespec GPR_CALLTYPE grpcsharp_request_call_context_deadline(
    const grpcsharp_request_call_context* ctx);

GPR_EXPORT const grpc_metadata_array* GPR_CALLTYPE
grpcsharp_request_call_context_request_metadata(
    const grpcsharp_request_call_context* ctx);

GPR_EXPORT void GPR_CALLTYPE grpcsharp_init(void);

GPR_EXPORT void GPR_CALLTYPE grpcsharp_shutdown(void);

GPR_EXPORT grpc_completion_queue* GPR_CALLTYPE
grpcsharp_completion_queue_create_async(void);

GPR_EXPORT grpc_completion_queue* GPR_CALLTYPE
grpcsharp_completion_queue_create_sync(void);

GPR_EXPORT void GPR_CALLTYPE
grpcsharp_completion_queue_shutdown(grpc_completion_queue* cq);

GPR_EXPORT void GPR_CALLTYPE
grpcsharp_completion_queue_destroy(grpc_completion_queue* cq);

GPR_EXPORT grpc_event GPR_CALLTYPE
grpcsharp_completion_queue_next(grpc_completion_queue* cq);

GPR_EXPORT grpc_event GPR_CALLTYPE
grpcsharp_completion_queue_pluck(grpc_completion_queue* cq, void* tag);

GPR_EXPORT grpc_channel* GPR_CALLTYPE
grpcsharp_insecure_channel_create(const char* target,
                                  const grpc_channel_args* args);

GPR_EXPORT void GPR_CALLTYPE grpcsharp_channel_destroy(grpc_channel* channel);

GPR_EXPORT grpc_connectivity_state GPR_CALLTYPE
grpcsharp_channel_check_connectivity_state(grpc_channel* channel,
                                           int32_t try_to_connect);

GPR_EXPORT void GPR_CALLTYPE grpcsharp_channel_watch_connectivity_state(
    grpc_channel* channel, grpc_connectivity_state last_observed_state,
    gpr_timespec deadline, grpc_completion_queue* cq,
    grpcsharp_batch_context* ctx);

GPR_EXPORT char* GPR_CALLTYPE
grpcsharp_channel_get_target(grpc_channel* channel);

GPR_EXPORT grpc_channel_args* GPR_CALLTYPE
grpcsharp_channel_args_create(size_t num_args);

GPR_EXPORT void GPR_CALLTYPE grpcsharp_channel_args_set_string(
    grpc_channel_args* args, size_t index, const char* key, const char* value);

GPR_EXPORT void GPR_CALLTYPE grpcsharp_channel_args_set_integer(
    grpc_channel_args* args, size_t index, const char* key, int value);

GPR_EXPORT void GPR_CALLTYPE
grpcsharp_channel_args_destroy(grpc_channel_args* args);

GPR_EXPORT grpc_call* GPR_CALLTYPE grpcsharp_channel_create_call(
    grpc_channel* channel, grpc_call* parent_call, uint32_t propagation_mask,
    grpc_completion_queue* cq, const char* method, const char* host,
    gpr_timespec deadline);

GPR_EXPORT grpc_call_error GPR_CALLTYPE grpcsharp_call_cancel(grpc_call* call);

GPR_EXPORT grpc_call_error GPR_CALLTYPE grpcsharp_call_cancel_with_status(
    grpc_call* call, grpc_status_code status, const char* description);

GPR_EXPORT char* GPR_CALLTYPE grpcsharp_call_get_peer(grpc_call* call);

GPR_EXPORT void GPR_CALLTYPE grpcsharp_call_destroy(grpc_call* call);

GPR_EXPORT grpc_call_error GPR_CALLTYPE grpcsharp_call_start_unary(
    grpc_call* call, grpcsharp_batch_context* ctx,
    grpc_slice_buffer* send_buffer, uint32_t write_flags,
    grpc_metadata_array* initial_metadata, uint32_t initial_metadata_flags);

GPR_EXPORT grpc_call_error GPR_CALLTYPE grpcsharp_test_call_start_unary_echo(
    grpc_call* call, grpcsharp_batch_context* ctx,
    grpc_slice_buffer* send_buffer, uint32_t write_flags,
    grpc_metadata_array* initial_metadata, uint32_t initial_metadata_flags);

GPR_EXPORT grpc_call_error GPR_CALLTYPE grpcsharp_call_start_client_streaming(
    grpc_call* call, grpcsharp_batch_context* ctx,
    grpc_metadata_array* initial_metadata, uint32_t initial_metadata_flags);

GPR_EXPORT grpc_call_error GPR_CALLTYPE grpcsharp_call_start_server_streaming(
    grpc_call* call, grpcsharp_batch_context* ctx,
    grpc_slice_buffer* send_buffer, uint32_t write_flags,
    grpc_metadata_array* initial_metadata, uint32_t initial_metadata_flags);

GPR_EXPORT grpc_call_error GPR_CALLTYPE grpcsharp_call_start_duplex_streaming(
    grpc_call* call, grpcsharp_batch_context* ctx,
    grpc_metadata_array* initial_metadata, uint32_t initial_metadata_flags);

GPR_EXPORT grpc_call_error GPR_CALLTYPE grpcsharp_call_recv_initial_metadata(
    grpc_call* call, grpcsharp_batch_context* ctx);

GPR_EXPORT grpc_call_error GPR_CALLTYPE grpcsharp_call_send_message(
    grpc_call* call, grpcsharp_batch_context* ctx,
    grpc_slice_buffer* send_buffer, uint32_t write_flags,
    int32_t send_empty_initial_metadata);

GPR_EXPORT grpc_call_error GPR_CALLTYPE grpcsharp_call_send_close_from_client(
    grpc_call* call, grpcsharp_batch_context* ctx);

GPR_EXPORT grpc_call_error GPR_CALLTYPE grpcsharp_call_send_status_from_server(
    grpc_call* call, grpcsharp_batch_context* ctx, grpc_status_code status_code,
    const char* status_details, size_t status_details_len,
    grpc_metadata_array* trailing_metadata, int32_t send_empty_initial_metadata,
    grpc_slice_buffer* optional_send_buffer, uint32_t write_flags);

GPR_EXPORT grpc_call_error GPR_CALLTYPE
grpcsharp_call_recv_message(grpc_call* call, grpcsharp_batch_context* ctx);

GPR_EXPORT grpc_call_error GPR_CALLTYPE
grpcsharp_call_start_serverside(grpc_call* call, grpcsharp_batch_context* ctx);

GPR_EXPORT grpc_call_error GPR_CALLTYPE grpcsharp_call_send_initial_metadata(
    grpc_call* call, grpcsharp_batch_context* ctx,
    grpc_metadata_array* initial_metadata);

GPR_EXPORT grpc_call_error GPR_CALLTYPE
grpcsharp_call_set_credentials(grpc_call* call, grpc_call_credentials* creds);

GPR_EXPORT gpr_timespec GPR_CALLTYPE gprsharp_now(gpr_clock_type clock_type);

GPR_EXPORT gpr_timespec GPR_CALLTYPE
gprsharp_inf_future(gpr_clock_type clock_type);

GPR_EXPORT gpr_timespec GPR_CALLTYPE
gprsharp_inf_past(gpr_clock_type clock_type);

GPR_EXPORT gpr_timespec GPR_CALLTYPE
gprsharp_convert_clock_type(gpr_timespec t, gpr_clock_type target_clock) ;

GPR_EXPORT int32_t GPR_CALLTYPE gprsharp_sizeof_timespec(void);

GPR_EXPORT void GPR_CALLTYPE gprsharp_free(void* p);

GPR_EXPORT grpc_server* GPR_CALLTYPE
grpcsharp_server_create(const grpc_channel_args* args);

GPR_EXPORT void GPR_CALLTYPE grpcsharp_server_register_completion_queue(
    grpc_server* server, grpc_completion_queue* cq);

GPR_EXPORT int32_t GPR_CALLTYPE grpcsharp_server_add_insecure_http2_port(
    grpc_server* server, const char* addr);

GPR_EXPORT int32_t GPR_CALLTYPE grpcsharp_server_add_secure_http2_port(
    grpc_server* server, const char* addr, grpc_server_credentials* creds);

GPR_EXPORT void GPR_CALLTYPE grpcsharp_server_start(grpc_server* server);

GPR_EXPORT void GPR_CALLTYPE grpcsharp_server_shutdown_and_notify_callback(
    grpc_server* server, grpc_completion_queue* cq,
    grpcsharp_batch_context* ctx);

GPR_EXPORT void GPR_CALLTYPE
grpcsharp_server_cancel_all_calls(grpc_server* server) ;

GPR_EXPORT void GPR_CALLTYPE grpcsharp_server_destroy(grpc_server* server);

GPR_EXPORT grpc_call_error GPR_CALLTYPE
grpcsharp_server_request_call(grpc_server* server, grpc_completion_queue* cq,
                              grpcsharp_request_call_context* ctx);

GPR_EXPORT void GPR_CALLTYPE grpcsharp_native_callback_dispatcher_init(
    grpcsharp_native_callback_dispatcher_func func);

GPR_EXPORT void GPR_CALLTYPE
grpcsharp_override_default_ssl_roots(const char* pem_root_certs);

GPR_EXPORT grpc_channel_credentials* GPR_CALLTYPE
grpcsharp_ssl_credentials_create(const char* pem_root_certs,
                                 const char* key_cert_pair_cert_chain,
                                 const char* key_cert_pair_private_key,
                                 void* verify_peer_callback_tag);

GPR_EXPORT void GPR_CALLTYPE
grpcsharp_channel_credentials_release(grpc_channel_credentials* creds);


GPR_EXPORT void GPR_CALLTYPE
grpcsharp_call_credentials_release(grpc_call_credentials* creds);

GPR_EXPORT grpc_channel* GPR_CALLTYPE grpcsharp_secure_channel_create(
    grpc_channel_credentials* creds, const char* target,
    const grpc_channel_args* args);

GPR_EXPORT grpc_server_credentials* GPR_CALLTYPE
grpcsharp_ssl_server_credentials_create(
    const char* pem_root_certs, const char** key_cert_pair_cert_chain_array,
    const char** key_cert_pair_private_key_array, size_t num_key_cert_pairs,
    grpc_ssl_client_certificate_request_type client_request_type);

GPR_EXPORT void GPR_CALLTYPE
grpcsharp_server_credentials_release(grpc_server_credentials* creds);

GPR_EXPORT grpc_channel_credentials* GPR_CALLTYPE
grpcsharp_composite_channel_credentials_create(
    grpc_channel_credentials* channel_creds,
    grpc_call_credentials* call_creds);

GPR_EXPORT grpc_call_credentials* GPR_CALLTYPE
grpcsharp_composite_call_credentials_create(grpc_call_credentials* creds1,
                                            grpc_call_credentials* creds2);

GPR_EXPORT void GPR_CALLTYPE grpcsharp_metadata_credentials_notify_from_plugin(
    grpc_credentials_plugin_metadata_cb cb, void* user_data,
    grpc_metadata_array* metadata, grpc_status_code status,
    const char* error_details);


GPR_EXPORT grpc_call_credentials* GPR_CALLTYPE
grpcsharp_metadata_credentials_create_from_plugin(void* callback_tag);

GPR_EXPORT grpc_auth_context* GPR_CALLTYPE
grpcsharp_call_auth_context(grpc_call* call);

GPR_EXPORT const char* GPR_CALLTYPE
grpcsharp_auth_context_peer_identity_property_name(
    const grpc_auth_context* ctx);

GPR_EXPORT grpc_auth_property_iterator GPR_CALLTYPE
grpcsharp_auth_context_property_iterator(const grpc_auth_context* ctx);

GPR_EXPORT const grpc_auth_property* GPR_CALLTYPE
grpcsharp_auth_property_iterator_next(grpc_auth_property_iterator* it);

GPR_EXPORT void GPR_CALLTYPE
grpcsharp_auth_context_release(grpc_auth_context* ctx);

GPR_EXPORT void GPR_CALLTYPE grpcsharp_redirect_log(grpcsharp_log_func func);

GPR_EXPORT grpc_slice_buffer* GPR_CALLTYPE grpcsharp_slice_buffer_create(void);

GPR_EXPORT void GPR_CALLTYPE
grpcsharp_slice_buffer_reset_and_unref(grpc_slice_buffer* buffer);

GPR_EXPORT void GPR_CALLTYPE
grpcsharp_slice_buffer_destroy(grpc_slice_buffer* buffer);

GPR_EXPORT size_t GPR_CALLTYPE
grpcsharp_slice_buffer_slice_count(grpc_slice_buffer* buffer);

GPR_EXPORT void GPR_CALLTYPE
grpcsharp_slice_buffer_slice_peek(grpc_slice_buffer* buffer, size_t index,
                                  size_t* slice_len, uint8_t** slice_data_ptr);

GPR_EXPORT void* GPR_CALLTYPE grpcsharp_slice_buffer_adjust_tail_space(
    grpc_slice_buffer* buffer, size_t available_tail_space,
    size_t requested_tail_space);

GPR_EXPORT const char* GPR_CALLTYPE grpcsharp_version_string();

GPR_EXPORT void GPR_CALLTYPE
grpcsharp_test_callback(test_callback_funcptr callback);

GPR_EXPORT void* GPR_CALLTYPE grpcsharp_test_nop(void* ptr);

GPR_EXPORT int32_t GPR_CALLTYPE grpcsharp_sizeof_grpc_event(void);

GPR_EXPORT void GPR_CALLTYPE
grpcsharp_test_override_method(const char* method_name, const char* variant);

#if defined(__cplusplus)
}
#endif

#endif

#include <VideoPlayer/DynamicallyLinkedMPV.h>

#include <stdexcept>
#include <sstream>
#include <cstdio>

std::optional<DynamicallyLinkedMPV> DynamicallyLinkedMPV::m_instance;

DynamicallyLinkedMPV::DynamicallyLinkedMPV() {

#ifdef _WIN32
    m_library.emplace(L"libmpv-2.dll");
#else
    try {
        m_library.emplace("libmpv.so");
    } catch(const std::exception &e) {
        fprintf(stderr, "failed to load libmpv library: %s\n", e.what());

        try {
            m_library.emplace("libmpv.so.2");
        } catch(const std::exception &e) {
            fprintf(stderr, "failed to load libmpv library: %s\n", e.what());

            m_library.emplace("libmpv.so.1");
        }
    }
#endif

    bindFunction("mpv_client_api_version", mpv_client_api_version);
    auto apiVersion = mpv_client_api_version();

    /*
     * Do not check the libmpv API version so strictly. We use a very small
     * subset of the API that seems to have been stable forever.
     */
#if 0
    if(apiVersion < MPV_CLIENT_API_VERSION || (apiVersion >> 16) != (MPV_CLIENT_API_VERSION >> 16)) {
        std::stringstream error;
        error << "Unsupported libmpv API version " << (apiVersion >> 16) << "." << (apiVersion & 0xFFFF) << ": must be at least "
              << (MPV_CLIENT_API_VERSION >> 16) << "." << (MPV_CLIENT_API_VERSION & 0xFFFF) << " and the major version must be strictly "
              << (MPV_CLIENT_API_VERSION >> 16);
        throw std::runtime_error(error.str());
    }
#endif

    printf("Loaded the libmpv library, API version: %lu.%lu, built for API version: %lu.%lu\n",
           apiVersion >> 16,
           apiVersion & 0xFFFF,
           MPV_CLIENT_API_VERSION >> 16,
           MPV_CLIENT_API_VERSION & 0xFFFF);

    bindFunction("mpv_create", mpv_create);
    bindFunction("mpv_request_log_messages", mpv_request_log_messages);
    bindFunction("mpv_initialize", mpv_initialize);
    bindFunction("mpv_wait_event", mpv_wait_event);
    bindFunction("mpv_command", mpv_command);
    bindFunction("mpv_get_property", mpv_get_property);
    bindFunction("mpv_set_property", mpv_set_property);
    bindFunction("mpv_observe_property", mpv_observe_property);
    bindFunction("mpv_render_context_create", mpv_render_context_create);
    bindFunction("mpv_render_context_free", mpv_render_context_free);
    bindFunction("mpv_render_context_render", mpv_render_context_render);
    bindFunction("mpv_terminate_destroy", mpv_terminate_destroy);
    bindFunction("mpv_error_string", mpv_error_string);

}

DynamicallyLinkedMPV::~DynamicallyLinkedMPV() = default;

const DynamicallyLinkedMPV &DynamicallyLinkedMPV::get() {
    if(!m_instance.has_value()) {
        throw std::logic_error("the MPV library has not been loaded");
    }

    return *m_instance;
}

void *DynamicallyLinkedMPV::bindFunction(const char *name) {
    auto result = m_library->getProcAddress(name);
    if(!result) {
        throw std::runtime_error("failed to locate the required symbol " + std::string(name));
    }
    return result;
}

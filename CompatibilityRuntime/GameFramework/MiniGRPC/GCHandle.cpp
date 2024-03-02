#include <MiniGRPC/GCHandle.h>

#include <stdexcept>

#include <il2cpp-api.h>

namespace minigrpc {


    GCHandle::GCHandle() noexcept : m_handle(0) {

    }

    GCHandle::GCHandle(Il2CppObject *object, Type type) : GCHandle() {
        if(!object)
            return;

        switch(type) {
            case Type::Normal:
                m_handle = il2cpp_gchandle_new(object, false);
                break;

            case Type::Pinned:
                m_handle = il2cpp_gchandle_new(object, true);
                break;

            case Type::Weak:
                m_handle = il2cpp_gchandle_new_weakref(object, false);
                break;

            case Type::WeakTrackedResurrection:
                m_handle = il2cpp_gchandle_new_weakref(object, true);
                break;

            default:
                throw std::logic_error("unsupported GC handle type");
        }

        if(!m_handle)
            throw std::runtime_error("failed to create the GC handle");
    }

    GCHandle::~GCHandle() {
        if(m_handle != 0) {
            il2cpp_gchandle_free(m_handle);
        }
    }


    GCHandle::GCHandle(GCHandle &&other) noexcept : GCHandle() {
        *this = std::move(other);
    }

    auto GCHandle::operator =(GCHandle &&other) noexcept -> GCHandle & {
        if(this != &other) {
            if(m_handle)
                il2cpp_gchandle_free(m_handle);

            m_handle = other.m_handle;
            other.m_handle = 0;
        }

        return *this;
    }

    Il2CppObject *GCHandle::get() const {
        if(m_handle == 0)
            return nullptr;

        return il2cpp_gchandle_get_target(m_handle);
    }
}

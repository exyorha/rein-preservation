#ifndef MINIGRPC_GC_HANDLE_H
#define MINIGRPC_GC_HANDLE_H

#include <il2cpp-api-types.h>

namespace minigrpc {

    class GCHandle {
    public:
        enum class Type {
            Normal,
            Pinned,
            Weak,
            WeakTrackedResurrection
        };

        GCHandle() noexcept;
        explicit GCHandle(Il2CppObject *object, Type type = Type::Normal);
        ~GCHandle();

        GCHandle(const GCHandle &other) = delete;
        GCHandle &operator =(const GCHandle &other) = delete;

        GCHandle(GCHandle &&other) noexcept;
        GCHandle &operator =(GCHandle &&other) noexcept;

        inline operator bool() const {
            return m_handle != 0;
        }

        Il2CppObject *get() const;

    private:
        uint32_t m_handle;
    };

}

#endif

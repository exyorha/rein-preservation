#ifndef LLSERVICES_NETWORKING_POSSIBLY_UNOWNED_POINTER_H
#define LLSERVICES_NETWORKING_POSSIBLY_UNOWNED_POINTER_H

#include <memory>
#include <variant>

namespace LLServices {

    template<typename T, typename Deleter = std::default_delete<T>>
    class PossiblyUnownedPointer {
    public:
        PossiblyUnownedPointer() noexcept = default;
        ~PossiblyUnownedPointer() = default;

        PossiblyUnownedPointer(const PossiblyUnownedPointer &other) = delete;
        PossiblyUnownedPointer &operator =(const PossiblyUnownedPointer &other) = delete;

        PossiblyUnownedPointer(PossiblyUnownedPointer &&other) noexcept = default;
        PossiblyUnownedPointer &operator =(PossiblyUnownedPointer &&other) noexcept = default;

        explicit PossiblyUnownedPointer(std::unique_ptr<T, Deleter> &&pointer) : m_storage(
            std::in_place_type_t<std::unique_ptr<T, Deleter>>(), std::move(pointer)) {

        }

        explicit PossiblyUnownedPointer(T *pointer) : m_storage(std::in_place_type_t<T *>(), pointer) {

        }

        PossiblyUnownedPointer &operator =(std::unique_ptr<T, Deleter> &&pointer) noexcept {
            return *this = PossiblyUnownedPointer(std::move(pointer));
        }

        PossiblyUnownedPointer &operator =(T *pointer) noexcept {
            return *this = PossiblyUnownedPointer(pointer);
        }

        inline T *get() const noexcept {
            return std::visit([](const auto &val) -> T * { return unwrapPointer(val); }, m_storage);
        }

        inline T &operator *() const noexcept {
            return *get();
        }

        inline T *operator ->() const noexcept {
            return get();
        }

        inline operator bool() const noexcept {
            return static_cast<bool>(get());
        }

    private:
        static inline T *unwrapPointer(T *ptr) {
            return ptr;
        }

        static inline T *unwrapPointer(const std::unique_ptr<T, Deleter> &ptr) {
            return ptr.get();
        }

        std::variant<std::unique_ptr<T, Deleter>, T *> m_storage;
    };

}

#endif

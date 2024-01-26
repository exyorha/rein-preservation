#ifndef ICALL_ARM_ARGUMENT_PACKER_H
#define ICALL_ARM_ARGUMENT_PACKER_H

#include <cstring>
#include <variant>
#include <vector>
#include <cstdint>

#include <ffi.h>
#include <il2cpp-api.h>

struct ARMIntegerLocation {
    size_t registerNumber;
};

struct ARMVectorLocation {
    size_t registerNumber;
};

struct ARMStackLocation {
    intptr_t offsetFromSP;
};

using ARMArgumentLocation = std::variant<std::monostate, ARMIntegerLocation, ARMVectorLocation, ARMStackLocation>;

class JITThreadContext;
class SavedICallContext;

void *getPointerToLocation(const ARMArgumentLocation &location, JITThreadContext &context);
void *getPointerToLocation(const ARMArgumentLocation &location, SavedICallContext &context);

class ARMSingleArgument {
public:
    ARMSingleArgument();
    ARMSingleArgument(const ARMArgumentLocation &location, ffi_type *ffi_type);
    ~ARMSingleArgument();

    ARMSingleArgument(const ARMSingleArgument &other);
    ARMSingleArgument &operator =(const ARMSingleArgument &other);

    ARMSingleArgument(ARMSingleArgument &&other) noexcept;
    ARMSingleArgument &operator =(ARMSingleArgument &&other) noexcept;

    inline ffi_type *type() const {
        return m_type;
    }

    template<typename T>
    inline void *getPointer(T &context) const {
        return getPointerToLocation(m_location, context);
    }

private:
    ARMArgumentLocation m_location;
    ffi_type *m_type;
};


class ARMArgumentSet {
public:
    ARMArgumentSet();
    ~ARMArgumentSet();

    ARMArgumentSet(const ARMArgumentSet &other);
    ARMArgumentSet &operator =(const ARMArgumentSet &other);

    ARMArgumentSet(ARMArgumentSet &&other) noexcept;
    ARMArgumentSet &operator =(ARMArgumentSet &&other) noexcept;

    ARMSingleArgument makeIntoSingleArgument();

    inline size_t size() const {
        return m_argumentTypes.size();
    }

    inline ffi_type **types() {
        return m_argumentTypes.data();
    }

    template<typename T>
    std::vector<void *> getPointers(T &context) const {
        std::vector<void *> pointers;
        pointers.reserve(m_argumentTypes.size());

        for(const auto &location: m_argumentLocations) {
            pointers.emplace_back(getPointerToLocation(location, context));
        }

        return pointers;
    }

private:
    friend class ARMArgumentPacker;

    std::vector<ARMArgumentLocation> m_argumentLocations;
    std::vector<ffi_type *> m_argumentTypes;
};

class ARMArgumentPacker {
public:
    ARMArgumentPacker();
    explicit ARMArgumentPacker(size_t estimatedArgumentCount);
    ~ARMArgumentPacker();

    ARMArgumentPacker(const ARMArgumentPacker &other) = delete;
    ARMArgumentPacker &operator =(const ARMArgumentPacker &other) = delete;

    void packPointer();
    void pack(const Il2CppType *type);

    void reserveFFIOnlyArgument(ffi_type *type);

    inline ARMArgumentSet &&finish() {
        return std::move(m_argumentSet);
    }


private:
    enum class ValueCategory {
        Integer,
        Void,
        Vector
    };

    static ValueCategory getValueCategory(const Il2CppType *type, ffi_type **ffi);

    ARMArgumentSet m_argumentSet;

    static constexpr size_t MaximumIntegerArguments = 8;
    static constexpr size_t MaximumVectorArguments = 8;

    size_t m_integerArgumentSlot;
    size_t m_vectorArgumentSlot;


};

#endif


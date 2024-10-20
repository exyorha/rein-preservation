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
class FFIStructureSynthesizer;

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

    template<typename T>
    void captureFromMachineContext(void *dest, T &context) const {
        if(m_type != &ffi_type_void) {
            memcpy(dest, getPointer(context), m_type->size);
        }
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

    inline size_t sizeOnARMSide() const {
        return m_argumentLocations.size();
    }

    inline ffi_type **types() {
        return m_argumentTypes.data();
    }

    template<typename T>
    void getPointers(T &context, void **&pointers) const {
        for(const auto &location: m_argumentLocations) {
            *pointers++ = getPointerToLocation(location, context);
        }
    }

    inline size_t argumentFrameSizeOnStack() const {
        return m_argumentFrameSizeOnStack;
    }

    template<typename T>
    void applyOntoMachineContext(void **arguments, T &context) const {
        auto ptr = arguments;
        auto types = m_argumentTypes.data();

        for(const auto &location: m_argumentLocations) {
            memcpy(getPointerToLocation(location, context), *ptr++, (*types)->size);
            types++;
        }
    }

private:
    friend class ARMArgumentPacker;

    std::vector<ARMArgumentLocation> m_argumentLocations;
    std::vector<ffi_type *> m_argumentTypes;
    size_t m_argumentFrameSizeOnStack;
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
    size_t m_currentSPOffset;
    static FFIStructureSynthesizer m_structureSynthesizer;


};

#endif


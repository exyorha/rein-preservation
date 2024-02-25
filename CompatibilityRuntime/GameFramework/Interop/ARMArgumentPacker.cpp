#include <Interop/SavedICallContext.h>
#include <Interop/ARMArgumentPacker.h>
#include <Interop/FFIStructureSynthesizer.h>

#include <Translator/JITThreadContext.h>

#include <support.h>

#include <il2cpp-blob.h>
#include <utility>
#include <variant>
#include <algorithm>

FFIStructureSynthesizer ARMArgumentPacker::m_structureSynthesizer;

template<typename T>
static inline void *getPointerToSpecificLocation(const std::monostate &voidLocation, T &context) {
    (void)context;
    return nullptr;
}

static inline void *getPointerToSpecificLocation(const ARMIntegerLocation &location, JITThreadContext &context) {
    return &context.gprs[location.registerNumber];
}

static inline void *getPointerToSpecificLocation(const ARMVectorLocation &location, JITThreadContext &context) {
    return &context.vectors[location.registerNumber];
}

static inline void *getPointerToSpecificLocation(const ARMStackLocation &location, JITThreadContext &context) {
    return reinterpret_cast<void *>(context.sp + location.offsetFromSP);
}

static inline void *getPointerToSpecificLocation(const ARMIntegerLocation &location, SavedICallContext &context) {
    return &context.intArgs[location.registerNumber];
}

static inline void *getPointerToSpecificLocation(const ARMVectorLocation &location, SavedICallContext &context) {
    return &context.vectorArgs[location.registerNumber];
}

static inline void *getPointerToSpecificLocation(const ARMStackLocation &location, SavedICallContext &context) {
    return reinterpret_cast<void *>(context.sp + location.offsetFromSP);
}

void *getPointerToLocation(const ARMArgumentLocation &location, JITThreadContext &context) {
    return std::visit([&context](const auto &locationType) -> void * { return getPointerToSpecificLocation(locationType, context); }, location);
}

void *getPointerToLocation(const ARMArgumentLocation &location, SavedICallContext &context) {
    return std::visit([&context](const auto &locationType) -> void * { return getPointerToSpecificLocation(locationType, context); }, location);
}

ARMSingleArgument::ARMSingleArgument() :
    m_location(std::in_place_type_t<std::monostate>()),
    m_type(&ffi_type_void) {

}

ARMSingleArgument::ARMSingleArgument(const ARMArgumentLocation &location, ffi_type *ffi_type) : m_location(location), m_type(ffi_type) {

}

ARMSingleArgument::~ARMSingleArgument() = default;

ARMSingleArgument::ARMSingleArgument(const ARMSingleArgument &other) = default;

ARMSingleArgument &ARMSingleArgument::operator =(const ARMSingleArgument &other) = default;

ARMSingleArgument::ARMSingleArgument(ARMSingleArgument &&other) noexcept = default;

ARMSingleArgument &ARMSingleArgument::operator =(ARMSingleArgument &&other) noexcept = default;

ARMArgumentSet::ARMArgumentSet() : m_argumentFrameSizeOnStack(0) {

}

ARMArgumentSet::~ARMArgumentSet() = default;

ARMArgumentSet::ARMArgumentSet(const ARMArgumentSet &other) = default;

ARMArgumentSet &ARMArgumentSet::operator =(const ARMArgumentSet &other) = default;

ARMArgumentSet::ARMArgumentSet(ARMArgumentSet &&other) noexcept = default;

ARMArgumentSet &ARMArgumentSet::operator =(ARMArgumentSet &&other) noexcept = default;

ARMSingleArgument ARMArgumentSet::makeIntoSingleArgument() {
    if(m_argumentTypes.size() != 1)
        throw std::logic_error("ARMArgumentSet::makeIntoSingleArgument: the set does not contain a single argument");

    return ARMSingleArgument(m_argumentLocations.front(), m_argumentTypes.front());
}

ARMArgumentPacker::ARMArgumentPacker() : m_integerArgumentSlot(0), m_vectorArgumentSlot(0), m_currentSPOffset(0) {

}

ARMArgumentPacker::ARMArgumentPacker(size_t reserve) : ARMArgumentPacker() {
    m_argumentSet.m_argumentLocations.reserve(reserve);
    m_argumentSet.m_argumentTypes.reserve(reserve);
}

ARMArgumentPacker::~ARMArgumentPacker() = default;

void ARMArgumentPacker::packPointer() {
    if(m_integerArgumentSlot < MaximumIntegerArguments) {
        m_argumentSet.m_argumentLocations.emplace_back(std::in_place_type_t<ARMIntegerLocation>(), m_integerArgumentSlot);
        m_argumentSet.m_argumentTypes.emplace_back(&ffi_type_pointer);
        m_integerArgumentSlot++;
    } else {
        panic("fetching integer args from stack is not implemented yet\n");
    }
}


void ARMArgumentPacker::pack(const Il2CppType *argumentType) {
    ffi_type *ffiType;
    auto category = getValueCategory(argumentType, &ffiType);

    if(category == ValueCategory::Integer) {
        if(ffiType->size == 0)
            throw std::logic_error("attempted to pack an uninitialized FFI type");

        auto alignmentForRegisterAllocation = std::clamp<unsigned int>(ffiType->alignment, 8, 16);

        if(alignmentForRegisterAllocation == 16) {
            /*
             * Round up the integer slot number.
             */
            m_integerArgumentSlot = (m_integerArgumentSlot + 1) & ~1;
        }

        auto numberOfRegisters = (ffiType->size + 7) / 8;

        if(numberOfRegisters > 2)
            panic("implement indirection (passing an argument of length %u - should be allocated elsewhere and passed as a pointer)",
                  ffiType->size);

        if(m_integerArgumentSlot + numberOfRegisters <= MaximumIntegerArguments) {
            /*
             * Allocate into registers.
             */

            m_argumentSet.m_argumentLocations.emplace_back(std::in_place_type_t<ARMIntegerLocation>(), m_integerArgumentSlot);
            m_integerArgumentSlot += numberOfRegisters;
        } else {
            /*
             * Allocate into stack.
             */

            // No more integer arguments can be allocated once we've allocated
            // anything onto the stack.
            m_integerArgumentSlot = MaximumIntegerArguments;

            /*
             * Align the stack.
             */
            m_currentSPOffset = (m_currentSPOffset + alignmentForRegisterAllocation - 1) & ~(alignmentForRegisterAllocation - 1);

            m_argumentSet.m_argumentLocations.emplace_back(std::in_place_type_t<ARMStackLocation>(), m_currentSPOffset);

            m_currentSPOffset = numberOfRegisters * sizeof(uint64_t);

            m_argumentSet.m_argumentFrameSizeOnStack = (m_currentSPOffset + 15) & ~15;
        }

    } else if(category == ValueCategory::Vector) {
        if(m_vectorArgumentSlot >= MaximumVectorArguments) {
            panic("fetching vector args from stack is not implemented yet\n");
        } else {
            m_argumentSet.m_argumentLocations.emplace_back(std::in_place_type_t<ARMVectorLocation>(), m_vectorArgumentSlot);
            m_vectorArgumentSlot++;
        }
    } else if(category == ValueCategory::Void) {
        m_argumentSet.m_argumentLocations.emplace_back(std::in_place_type_t<std::monostate>());
    } else {
        panic("unsupported argument return category\n");
    }

    m_argumentSet.m_argumentTypes.emplace_back(ffiType);
}

auto ARMArgumentPacker::getValueCategory(const Il2CppType *type, ffi_type **ffi) -> ValueCategory {
    auto typeCategory = il2cpp_type_get_type(type);
    auto typeName = il2cpp_type_get_name(type);

    auto ptr = il2cpp_type_is_pointer_type(type);
    auto ref = il2cpp_type_is_byref(type);

#if 0
    printf("sorting out type: '%s', category: %d; pointer: %d, byref: %d\n", typeName, typeCategory, ptr, ref);
#endif

    if(
        ptr ||
        ref ||
        typeCategory == IL2CPP_TYPE_CLASS ||
        typeCategory == IL2CPP_TYPE_STRING ||
        typeCategory == IL2CPP_TYPE_SZARRAY ||
        typeCategory == IL2CPP_TYPE_I || // intptr_t, technically not a pointer but it doesn't matter
        typeCategory == IL2CPP_TYPE_OBJECT ||
        typeCategory == IL2CPP_TYPE_GENERICINST /* I think this *should* be correct but I'm not sure */
    ) {
        *ffi = &ffi_type_pointer;
        return ValueCategory::Integer;
    } else if(typeCategory == IL2CPP_TYPE_VOID) {
        *ffi = &ffi_type_void;
        return ValueCategory::Void;
    } else if(typeCategory == IL2CPP_TYPE_BOOLEAN) {
        *ffi = &ffi_type_uint8;
        return ValueCategory::Integer;
    } else if(typeCategory == IL2CPP_TYPE_U1) {
        *ffi = &ffi_type_uint8;
        return ValueCategory::Integer;

    } else if(typeCategory == IL2CPP_TYPE_U2) {
        *ffi = &ffi_type_uint16;
        return ValueCategory::Integer;

    } else if(typeCategory == IL2CPP_TYPE_CHAR) {
        *ffi = &ffi_type_uint16;
        return ValueCategory::Integer;

    } else if(typeCategory == IL2CPP_TYPE_U4) {
        *ffi = &ffi_type_uint32;
        return ValueCategory::Integer;

    } else if(typeCategory == IL2CPP_TYPE_I4) {
        *ffi = &ffi_type_sint32;
        return ValueCategory::Integer;

    } else if(typeCategory == IL2CPP_TYPE_I8) {
        *ffi = &ffi_type_sint64;
        return ValueCategory::Integer;

    } else if(typeCategory == IL2CPP_TYPE_U8) {
        *ffi = &ffi_type_uint64;
        return ValueCategory::Integer;
    } else if(typeCategory == IL2CPP_TYPE_VALUETYPE) {

        auto typeClass = il2cpp_type_get_class_or_element_class(type);
        *ffi = m_structureSynthesizer.createFFITypeForValueType(typeClass);

        return ValueCategory::Integer;

    } else if(typeCategory == IL2CPP_TYPE_R4) {
        *ffi = &ffi_type_float;
        return ValueCategory::Vector;

    } else if(typeCategory == IL2CPP_TYPE_R8) {
        *ffi = &ffi_type_double;
        return ValueCategory::Vector;

    } else {
        panic("unsupported argument type\n");
    }

}


void ARMArgumentPacker::reserveFFIOnlyArgument(ffi_type *type) {
    m_argumentSet.m_argumentTypes.emplace_back(type);
}

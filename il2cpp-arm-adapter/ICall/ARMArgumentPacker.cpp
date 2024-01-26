#include <ICall/SavedICallContext.h>
#include <ICall/ARMArgumentPacker.h>

#include <Translator/JITThreadContext.h>

#include <support.h>

#include <il2cpp-blob.h>
#include <variant>

template<typename T>
static inline void *getPointerToLocation(const std::monostate &voidLocation, T &context) {
    (void)context;
    return nullptr;
}

static inline void *getPointerToLocation(const ARMIntegerLocation &location, JITThreadContext &context) {
    return &context.gprs[location.registerNumber];
}

static inline void *getPointerToLocation(const ARMVectorLocation &location, JITThreadContext &context) {
    return &context.vectors[location.registerNumber];
}

static inline void *getPointerToLocation(const ARMIntegerLocation &location, SavedICallContext &context) {
    return &context.intArgs[location.registerNumber];
}

static inline void *getPointerToLocation(const ARMVectorLocation &location, SavedICallContext &context) {
    return &context.vectorArgs[location.registerNumber];
}

void *getPointerToLocation(const ARMArgumentLocation &location, JITThreadContext &context) {
    return std::visit([&context](const auto &locationType) -> void * { return getPointerToLocation(locationType, context); }, location);
}

void *getPointerToLocation(const ARMArgumentLocation &location, SavedICallContext &context) {
    return std::visit([&context](const auto &locationType) -> void * { return getPointerToLocation(locationType, context); }, location);
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

ARMArgumentSet::ARMArgumentSet() = default;

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

ARMArgumentPacker::ARMArgumentPacker() : m_integerArgumentSlot(0), m_vectorArgumentSlot(0) {

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
        if(m_integerArgumentSlot >= MaximumIntegerArguments) {
            panic("fetching integer args from stack is not implemented yet\n");
        } else {
            m_argumentSet.m_argumentLocations.emplace_back(std::in_place_type_t<ARMIntegerLocation>(), m_integerArgumentSlot);
            m_integerArgumentSlot++;
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

    printf("sorting out type: '%s', category: %d; pointer: %d, byref: %d\n", typeName, typeCategory, ptr, ref);

    if(typeCategory == IL2CPP_TYPE_VOID) {
        *ffi = &ffi_type_void;
        return ValueCategory::Void;
    } else if(typeCategory == IL2CPP_TYPE_BOOLEAN) {
        *ffi = &ffi_type_uint8;
        return ValueCategory::Integer;

    } else if(
        ptr ||
        ref ||
        typeCategory == IL2CPP_TYPE_CLASS ||
        typeCategory == IL2CPP_TYPE_STRING ||
        typeCategory == IL2CPP_TYPE_SZARRAY ||
        typeCategory == IL2CPP_TYPE_I || // intptr_t, technically not a pointer but it doesn't matter
        typeCategory == IL2CPP_TYPE_OBJECT
    ) {
        *ffi = &ffi_type_pointer;
        return ValueCategory::Integer;
    } else if(typeCategory == IL2CPP_TYPE_I4) {
        *ffi = &ffi_type_sint32;
        return ValueCategory::Integer;

    } else if(typeCategory == IL2CPP_TYPE_VALUETYPE) {
        auto typeClass = il2cpp_type_get_class_or_element_class(type);
        uint32_t valueAlign;
        auto valueSize = il2cpp_class_value_size(typeClass, &valueAlign);

        if(valueSize == 4 && valueAlign == 4) {
            *ffi = &ffi_type_uint32;
            return ValueCategory::Integer;
        } else {

            panic("value type passed by value; size: %u, align: %u\n", valueSize, valueAlign);
        }

    } else if(typeCategory == IL2CPP_TYPE_R4) {
        *ffi = &ffi_type_float;
        return ValueCategory::Vector;

    } else {
        panic("unsupported argument type\n");
    }

}


#include "ICallType.h"

#include <stdexcept>

#include <il2cpp-blob.h>

ICallType::ICallType(TypeCategory category) : m_typeCategory(category) {

}

ICallType::TypeCategory ICallType::classifyType(const Il2CppType *type) {

    auto ptr = il2cpp_type_is_pointer_type(type);
    auto ref = il2cpp_type_is_byref(type);

    auto typeCategory = il2cpp_type_get_type(type);

    if(
        ptr ||
        ref ||
        typeCategory == IL2CPP_TYPE_CLASS ||
        typeCategory == IL2CPP_TYPE_STRING ||
        typeCategory == IL2CPP_TYPE_SZARRAY ||
        typeCategory == IL2CPP_TYPE_ARRAY ||
        typeCategory == IL2CPP_TYPE_OBJECT ||
        typeCategory == IL2CPP_TYPE_GENERICINST) { /* I think this *should* be correct but I'm not sure */
        return TypeCategory::Pointer;
    } else if(typeCategory == IL2CPP_TYPE_I) {
        return TypeCategory::IntPtr;
    } else if(typeCategory == IL2CPP_TYPE_VOID) {
        return TypeCategory::Void;
    } else if(typeCategory == IL2CPP_TYPE_BOOLEAN) {

        return TypeCategory::Bool;

    } else if(typeCategory == IL2CPP_TYPE_U1) {
        return TypeCategory::UInt8;

    } else if(typeCategory == IL2CPP_TYPE_U2) {
        return TypeCategory::UInt16;

    } else if(typeCategory == IL2CPP_TYPE_CHAR) {
        return TypeCategory::UInt16;

    } else if(typeCategory == IL2CPP_TYPE_U4) {
        return TypeCategory::UInt32;

    } else if(typeCategory == IL2CPP_TYPE_I1) {
        return TypeCategory::Int8;

    } else if(typeCategory == IL2CPP_TYPE_I2) {
        return TypeCategory::Int16;

    } else if(typeCategory == IL2CPP_TYPE_I4) {
        return TypeCategory::Int32;

    } else if(typeCategory == IL2CPP_TYPE_I8) {
        return TypeCategory::Int64;

    } else if(typeCategory == IL2CPP_TYPE_U8) {
        return TypeCategory::UInt64;

    } else if(typeCategory == IL2CPP_TYPE_VALUETYPE) {

        auto typeClass = il2cpp_type_get_class_or_element_class(type);

        if(il2cpp_class_is_enum(typeClass)) {
            return classifyType(il2cpp_class_enum_basetype(typeClass));
        } else {
            throw std::runtime_error("attempted to pass a non-enum class by value in an icall: " + std::string(il2cpp_type_get_name(type)));
        }

    } else if(typeCategory == IL2CPP_TYPE_R4) {
        return TypeCategory::Float;

    } else if(typeCategory == IL2CPP_TYPE_R8) {
        return TypeCategory::Double;

    } else {
        throw std::runtime_error("unsupported argument type, type category: " + std::to_string(static_cast<int>(typeCategory)));
    }
}

ICallType::ICallType(const Il2CppType *type) : m_typeCategory(classifyType(type)) {

}

ICallType::~ICallType() = default;

ICallType::ICallType(ICallType &&other) noexcept = default;

ICallType &ICallType::operator =(ICallType &&other) noexcept = default;

ICallType ICallType::makePointer() {
    return ICallType(TypeCategory::Pointer);
}

const char *ICallType::getCType() const {
    switch(m_typeCategory) {
        case TypeCategory::Void:
            return "void";

        case TypeCategory::Pointer:
            return "void *";

        case TypeCategory::IntPtr:
            return "intptr_t";

        case TypeCategory::Bool:
            return "bool";

        case TypeCategory::UInt8:
            return "uint8_t";

        case TypeCategory::UInt16:
            return "uint16_t";

        case TypeCategory::UInt32:
            return "uint32_t";

        case TypeCategory::UInt64:
            return "uint64_t";

        case TypeCategory::Int8:
            return "int8_t";

        case TypeCategory::Int16:
            return "int16_t";

        case TypeCategory::Int32:
            return "int32_t";

        case TypeCategory::Int64:
            return "int64_t";

        case TypeCategory::Float:
            return "float";

        case TypeCategory::Double:
            return "double";

        default:
            throw std::runtime_error("unknown type category");
    }
}


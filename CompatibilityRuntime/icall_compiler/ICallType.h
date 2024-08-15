#ifndef ICALL_TYPE_H
#define ICALL_TYPE_H

#include <il2cpp-api.h>

class ICallType {
public:
    enum class TypeCategory {
        Void,
        Pointer,
        IntPtr,
        Bool,
        UInt8,
        UInt16,
        UInt32,
        UInt64,
        Int8,
        Int16,
        Int32,
        Int64,
        Float,
        Double
    };

private:
    explicit ICallType(TypeCategory category);

public:
    explicit ICallType(const Il2CppType *type);
    ~ICallType();

    ICallType(const ICallType &other) = delete;
    ICallType &operator =(const ICallType &other) = delete;

    ICallType(ICallType &&other) noexcept;
    ICallType &operator =(ICallType &&other) noexcept;

    static ICallType makePointer();

    inline bool isVoid() const {
        return m_typeCategory == TypeCategory::Void;
    }

    inline bool isVector() const {
        return m_typeCategory == TypeCategory::Float || m_typeCategory == TypeCategory::Double;
    }

    const char *getCType() const;

private:
    static TypeCategory classifyType(const Il2CppType *type);

    TypeCategory m_typeCategory;
};

#endif

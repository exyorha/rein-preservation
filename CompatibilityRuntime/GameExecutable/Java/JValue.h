#ifndef JAVA_JVALUE_H
#define JAVA_JVALUE_H

#include "Il2CppUtilities.h"

#include <cstdint>
#include <memory>

class JNIObject;

/*
 * Maps to UnityEngine.jvalue
 */
union JValue {
    bool z;
    int8_t b;
    char16_t c;
    int16_t s;
    int32_t i;
    int64_t j;
    float f;
    double d;
    intptr_t l;

    inline explicit operator bool() const {
        return z;
    }

    inline explicit operator int8_t() const {
        return b;
    }

    inline explicit operator char16_t() const {
        return c;
    }

    inline explicit operator int16_t() const {
        return s;
    }

    inline explicit operator int32_t() const {
        return i;
    }

    inline explicit operator int64_t() const {
        return j;
    }

    inline explicit operator float() const {
        return f;
    }

    inline explicit operator double() const {
        return d;
    }

    template<typename Type, typename = typename std::enable_if<std::is_base_of<JNIObject, Type>::value>::type>
    inline explicit operator std::shared_ptr<Type>() const;

    explicit operator const std::string &() const;
};

static_assert(sizeof(JValue) == 8 && alignof(JValue) <= 8 && std::is_standard_layout<JValue>::value,
              "JValue must match the layout of the corresponding unboxed .NET type");

using JValueArray = ArrayWrapper<JValue>;

#endif

#ifndef JAVA_JNI_OBJECT_H
#define JAVA_JNI_OBJECT_H

#include <memory>

#include <il2cpp-api.h>
#include <Java/JValue.h>

class JNIClass;

class JNIObject : public std::enable_shared_from_this<JNIObject> {
protected:
    explicit JNIObject(const std::shared_ptr<JNIClass> &objectClass);
    virtual ~JNIObject();

public:
    JNIObject(const JNIObject &other) = delete;
    JNIObject &operator =(const JNIObject &other) = delete;

    static std::shared_ptr<JNIClass> makeClass();

    virtual std::shared_ptr<JNIObject> toString();

    inline const std::shared_ptr<JNIClass> &objectClass() const {
        return m_objectClass;
    }

protected:
    static std::shared_ptr<JNIClass> parent(const std::string_view &name);

    template<typename T>
    inline static void nonNull(const std::shared_ptr<T> &value) {
        if(!value)
            unexpectedNullValue();
    }

private:
    [[noreturn]] static void unexpectedNullValue();

    std::shared_ptr<JNIClass> m_objectClass;
};

#endif


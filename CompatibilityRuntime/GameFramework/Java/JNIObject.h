#ifndef JAVA_JNI_OBJECT_H
#define JAVA_JNI_OBJECT_H

#include <memory>

#include <il2cpp-api.h>

class JNIClass;

class JNIObject : public std::enable_shared_from_this<JNIObject> {
protected:
    JNIObject();
    virtual ~JNIObject();

public:
    JNIObject(const JNIObject &other) = delete;
    JNIObject &operator =(const JNIObject &other) = delete;

    static std::shared_ptr<JNIClass> makeClass();

    virtual std::shared_ptr<JNIObject> toString(Il2CppArray *args);

protected:
    static std::shared_ptr<JNIClass> parent(const std::string_view &name);
};

#endif


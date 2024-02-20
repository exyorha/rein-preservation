#ifndef JNI_STRING_H
#define JNI_STRING_H

#include <Java/JNIObject.h>

class JNIString final : public JNIObject {
public:
    explicit JNIString(const std::string_view &string = {});
    ~JNIString();

    std::shared_ptr<JNIObject> toString() override;

    static std::shared_ptr<JNIClass> makeClass();

    inline const std::string &string() const {
        return m_string;
    }

private:
    std::string m_string;
};


#endif

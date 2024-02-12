#ifndef JAVA_UNI_WEB_VIEW_INTERFACE_H
#define JAVA_UNI_WEB_VIEW_INTERFACE_H

#include <Java/JNIClass.h>

class UniWebViewInterface final : public JNIObject {
public:

    static std::shared_ptr<JNIClass> makeClass();

};

#endif



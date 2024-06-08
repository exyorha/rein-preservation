#ifndef JAVA_NATIVE_GALLERY_PERMISSION_RECEIVER_H
#define JAVA_NATIVE_GALLERY_PERMISSION_RECEIVER_H

#include <Java/JNIClass.h>

class NativeGalleryPermissionReceiver final : public JNIObject {
public:

    static std::shared_ptr<JNIClass> makeClass();

};

#endif



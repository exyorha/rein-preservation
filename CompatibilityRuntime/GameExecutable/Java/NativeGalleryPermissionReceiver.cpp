 #include <Java/NativeGalleryPermissionReceiver.h>

std::shared_ptr<JNIClass> NativeGalleryPermissionReceiver::makeClass() {
    auto co = std::make_shared<JNIClass>("com/yasirkula/unity/NativeGalleryPermissionReceiver", parent("java/lang/Object"));

    return co;
}

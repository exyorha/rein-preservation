#ifndef JAVA_AV_PRO_MOBILE_VIDEO_H
#define JAVA_AV_PRO_MOBILE_VIDEO_H

#include <Java/JNIObject.h>

class AVProMobileVideo final : public JNIObject {
public:
    AVProMobileVideo();
    ~AVProMobileVideo();

    AVProMobileVideo(const AVProMobileVideo &other) = delete;
    AVProMobileVideo &operator =(const AVProMobileVideo &other) = delete;

    static std::shared_ptr<JNIClass> makeClass();

    std::shared_ptr<JNIObject> GetPluginVersion();

    void SetContext(std::shared_ptr<JNIObject> androidContextObject);

    std::shared_ptr<JNIObject> CreatePlayer(int32_t arg1, bool arg2, bool arg3, int32_t arg4, bool arg5);
    void DestroyPlayer(int32_t playerIndex);

    static void Deinitialise();

    /*
     * Called directly through AVLocal (see AVProVideoNativeBypass)
     */
    static int32_t GetWidth(int32_t playerIndex);
    static int32_t GetHeight(int32_t playerIndex);
    static int32_t GetTextureHandle(int32_t playerIndex);
    static int64_t GetDuration(int32_t playerIndex);
    static int32_t GetLastErrorCode(int32_t playerIndex);
    static int32_t GetFrameCount(int32_t playerIndex);
    static float GetVideoDisplayRate(int32_t playerIndex);
    static bool CanPlay(int32_t playerIndex);

private:
    static std::shared_ptr<JNIObject> create();
};

#endif


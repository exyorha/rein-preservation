abi=arm64-v8a
ANDROID_NDK_VERSION=21.4.7075529
ANDROID_PLATFORM=21

cmake -S . -B bionic-build -DCMAKE_TOOLCHAIN_FILE=/opt/android-sdk/ndk/${ANDROID_NDK_VERSION}/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=$abi \
    -DANDROID_PLATFORM=android-${ANDROID_PLATFORM} \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo

cmake --build bionic-build

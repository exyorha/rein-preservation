set -e
cmake \
    -S il2cpp-arm-adapter \
    -B build -G "Kate - Ninja" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DCMAKE_INSTALL_PREFIX="$(realpath -- graft)"
ln -sf ../build/compile_commands.json il2cpp-arm-adapter
cmake --build build
cmake --install build --component GameAssembly


abi=arm64-v8a
ANDROID_NDK_VERSION=21.4.7075529
ANDROID_PLATFORM=21

cmake -S LogAES -B LogAES-build$abi -DCMAKE_TOOLCHAIN_FILE=/opt/android-sdk/ndk/${ANDROID_NDK_VERSION}/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=$abi \
    -DANDROID_PLATFORM=android-${ANDROID_PLATFORM} \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DCMAKE_INSTALL_PREFIX=${PWD}/replacement-apk-files

cmake --build LogAES-build$abi --target install/strip

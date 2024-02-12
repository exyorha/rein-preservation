set -e
cmake \
    -S CompatibilityRuntime \
    -B CompatibilityRuntime-build -G "Kate - Ninja" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DCMAKE_INSTALL_PREFIX="$(realpath -- graft)"
ln -sf ../CompatibilityRuntime-build/compile_commands.json CompatibilityRuntime
cmake --build CompatibilityRuntime-build
cmake --install CompatibilityRuntime-build --component GameAssembly

mkdir -p windows-build-deps dl

windows_boost_version=boost_1_84_0

if [ ! -d "windows-build-deps/${windows_boost_version}" ]; then
    if [ ! -f "dl/${windows_boost_version}.7z" ]; then
        wget -O "dl/${windows_boost_version}.7z.part" https://boostorg.jfrog.io/artifactory/main/release/1.84.0/source/${windows_boost_version}.7z
        mv "dl/${windows_boost_version}.7z.part" "dl/${windows_boost_version}.7z"
    fi

    mkdir -p windows-build-deps/part
    7z -owindows-build-deps/part x "dl/${windows_boost_version}.7z"
    mv "windows-build-deps/part/${windows_boost_version}" "windows-build-deps/${windows_boost_version}"
fi

mkdir -p windows-build-root-path

cmake -S CompatibilityRuntime -B CompatibilityRuntime-mingw-build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE \
    -DCMAKE_INSTALL_PREFIX="$(realpath -- graft)" \
    -DCMAKE_TOOLCHAIN_FILE="$(realpath -- toolchain-windows-x86_64.txt)" \
    -DCMAKE_FIND_ROOT_PATH="$(realpath -- windows-build-root-path)" \
    -DBoost_INCLUDE_DIR="$(realpath -- "windows-build-deps/${windows_boost_version}")" \
    -G "Ninja"

cmake --build CompatibilityRuntime-mingw-build

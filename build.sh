#!/bin/bash
set -e

download_and_unpack_windows_dependency() {
    local url="$1"
    local basename="$(basename -- "$url")"
    local stem="$(basename -s ".7z" -- "$(basename -s ".tar.gz" -- "$url")")"

    echo "$url" "$basename" "$stem"

    if [ ! -d "windows-build-deps/${stem}" ]; then
        if [ ! -f "dl/${basename}" ]; then
            wget -O "dl/${basename}.part" "${url}"
            mv "dl/${basename}.part" "dl/${basename}"
        fi

        mkdir -p windows-build-deps/part

        if [[ $basename = *.7z ]]; then
            7z -owindows-build-deps/part x "dl/${basename}"
        else
            tar xf "dl/${basename}" -C windows-build-deps/part
        fi
        mv "windows-build-deps/part/${stem}" "windows-build-deps"
    fi
}

cmake \
    -S CompatibilityRuntime \
    -B CompatibilityRuntime-build -G "Kate - Ninja" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DCMAKE_INSTALL_PREFIX="$(realpath -- graft)"
ln -sf ../CompatibilityRuntime-build/compile_commands.json CompatibilityRuntime
cmake --build CompatibilityRuntime-build
cmake --install CompatibilityRuntime-build --component GameAssembly

cmake \
    -S GameServer \
    -B GameServer-build -G "Kate - Ninja" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DCMAKE_INSTALL_PREFIX="$(realpath -- graft)"
ln -sf ../GameServer-build/compile_commands.json GameServer
cmake --build GameServer-build
cmake --install GameServer-build --component GameServer

#exit 0

mkdir -p windows-build-deps dl

windows_boost_version=boost_1_84_0
windows_ffi_version=libffi-3.4.4

download_and_unpack_windows_dependency "https://boostorg.jfrog.io/artifactory/main/release/1.84.0/source/${windows_boost_version}.7z"
download_and_unpack_windows_dependency "https://github.com/libffi/libffi/releases/download/v3.4.4/${windows_ffi_version}.tar.gz"

mkdir -p windows-build-deps/libffi-build

winprefix="$(realpath -- windows-build-root-path)"

if [ ! -f windows-build-deps/libffi-build/config.status ]; then
    (cd windows-build-deps/libffi-build && ../${windows_ffi_version}/configure \
        --build="$(gcc -dumpmachine)" \
        --host=x86_64-w64-mingw32 \
        --prefix="${winprefix}" \
        --enable-static --disable-shared --with-pic --disable-docs)
fi

make -C windows-build-deps/libffi-build install

mkdir -p windows-build-root-path

cmake -S CompatibilityRuntime -B CompatibilityRuntime-mingw-build \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE \
    -DCMAKE_INSTALL_PREFIX="$(realpath -- graft)" \
    -DCMAKE_TOOLCHAIN_FILE="$(realpath -- toolchain-windows-x86_64.txt)" \
    -DCMAKE_FIND_ROOT_PATH="$(realpath -- windows-build-root-path)" \
    -DBoost_INCLUDE_DIR="$(realpath -- "windows-build-deps/${windows_boost_version}")" \
    -G "Ninja"

cmake --build CompatibilityRuntime-mingw-build
cmake --install CompatibilityRuntime-mingw-build --component GameAssembly

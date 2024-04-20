#!/bin/bash
set -e

download_and_unpack_windows_dependency() {
    local url="$1"
    local basename="$(basename -- "$url")"
    local stem

    if [ -z "$2" ]; then
        stem="$(basename -s ".7z" -- "$(basename -s ".tar.gz" -- "$url")")"
    else
        stem="$2"
    fi

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

prepend_include() {
    local include="$1"
    local filename="$2"

    if ! grep -Fq -- "${include}" "${filename}"; then

        echo "${include}" > "${filename}.new"
        cat "${filename}" >> "${filename}.new"
        mv "${filename}.new" "${filename}"
    fi
}

mkdir -p dl

cmake \
    -S . \
    -B build -G "Kate - Ninja" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DCMAKE_INSTALL_PREFIX="$(realpath -- reincarnation)" \
    -DCEF_ROOT="$(realpath -- cef_binary_121.3.13+g5c4a81b+chromium-121.0.6167.184_linux64_minimal)" \
    -DANDROID_NDK_ROOT=/opt/android-sdk/ndk/21.4.7075529
ln -sf build/compile_commands.json compile_commands.json
cmake --build build
cmake --install build --component GameAssembly
cmake --install build --component GameServer

(cd winmpv && git ls-tree --name-only --full-name HEAD | tar cz --files-from=-) > reincarnation/libmpv-lgpl-compliance.tgz

if [ ! -f winmpv/windows-libmpv.tar ]; then
    echo "winmpv/windows-libmpv.tar is not present. Please run ./build.sh in the winmpv directory."
    exit 1
fi

if [ ! -f reincarnation/libmpv-2.dll ]; then
    tar -C reincarnation --strip-components=1 -xf winmpv/windows-libmpv.tar bin/libmpv-2.dll
fi

mkdir -p windows-build-deps

windows_boost_version=boost_1_84_0
windows_ffi_version=libffi-3.4.4
windows_abseil_version=abseil-cpp-20240116.1
windows_protobuf_version=protobuf-25.2
windows_zlib_version=zlib-1.3.1

download_and_unpack_windows_dependency "https://boostorg.jfrog.io/artifactory/main/release/1.84.0/source/${windows_boost_version}.7z"
download_and_unpack_windows_dependency "https://github.com/libffi/libffi/releases/download/v3.4.4/${windows_ffi_version}.tar.gz"
download_and_unpack_windows_dependency "https://github.com/abseil/abseil-cpp/releases/download/20240116.1/${windows_abseil_version}.tar.gz"
download_and_unpack_windows_dependency "https://github.com/protocolbuffers/protobuf/releases/download/v25.2/${windows_protobuf_version}.tar.gz"
download_and_unpack_windows_dependency "https://www.zlib.net/${windows_zlib_version}.tar.gz"

mkdir -p windows-build-deps/libffi-build windows-build-deps/c-ares-build

winprefix="$(realpath -- windows-build-root-path)"

mkdir -p "$winprefix"

if [ ! -f "${winprefix}/libmpv_installed" ]; then
    tar -C "${winprefix}" -xf winmpv/windows-libmpv.tar include lib
    sed "s!^prefix=.*\$!prefix=${winprefix}!;/\.private: /d" -i -- ${winprefix}/lib/pkgconfig/mpv.pc
    touch "${winprefix}/libmpv_installed"
fi

if [ ! -e "${winprefix}/bin/x86_64-w64-mingw32-pkg-config" ]; then
    cat > "${winprefix}/bin/x86_64-w64-mingw32-pkg-config" <<EOF
#!/bin/sh

export PKG_CONFIG_LIBDIR="${winprefix}/lib/pkgconfig"
exec pkgconf "\$@"
EOF

    chmod +x "${winprefix}/bin/x86_64-w64-mingw32-pkg-config"
fi

if [ ! -f "${winprefix}/ffi_installed" ]; then

    if [ ! -f windows-build-deps/libffi-build/config.status ]; then
        (cd windows-build-deps/libffi-build && ../${windows_ffi_version}/configure \
            --build="$(gcc -dumpmachine)" \
            --host=x86_64-w64-mingw32 \
            --prefix="${winprefix}" \
            --enable-static --disable-shared --with-pic --disable-docs)
    fi

    make -C windows-build-deps/libffi-build install

    touch "${winprefix}/ffi_installed"
fi

if [ ! -f "${winprefix}/absl_installed" ]; then

    cmake -S "windows-build-deps/${windows_abseil_version}" -B "windows-build-deps/abseil_build" \
        -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="${winprefix}" \
        -DCMAKE_TOOLCHAIN_FILE="$(realpath -- toolchain-windows-x86_64.txt)" \
        -DCMAKE_FIND_ROOT_PATH="${winprefix}" \
        -DBUILD_TESTING=OFF -DBUILD_SHARED_LIBS=OFF

    cmake --build "windows-build-deps/abseil_build" --parallel 8 --target install

    touch "${winprefix}/absl_installed"
fi

if [ ! -f "${winprefix}/zlib_installed" ]; then

    cmake -S "windows-build-deps/${windows_zlib_version}" -B "windows-build-deps/zlib_build" \
        -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="${winprefix}" \
        -DCMAKE_TOOLCHAIN_FILE="$(realpath -- toolchain-windows-x86_64.txt)" \
        -DCMAKE_FIND_ROOT_PATH="${winprefix}" \
        -DZLIB_BUILD_EXAMPLES=OFF

    cmake --build "windows-build-deps/zlib_build" --parallel 8 --target install

    # We don't want the shared libzlib to be used.
    rm -f "${winprefix}/bin/libzlib.dll"
    rm -f "${winprefix}/lib/libzlib.dll.a"

    touch "${winprefix}/zlib_installed"
fi

if [ ! -f "${winprefix}/protobuf_installed" ]; then
    cmake -S "windows-build-deps/${windows_protobuf_version}" -B "windows-build-deps/protobuf_build" \
        -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="${winprefix}" \
        -DCMAKE_TOOLCHAIN_FILE="$(realpath -- toolchain-windows-x86_64.txt)" \
        -DCMAKE_FIND_ROOT_PATH="${winprefix}" \
        -DBUILD_SHARED_LIBS=OFF -Dprotobuf_BUILD_TESTS=OFF -Dprotobuf_ABSL_PROVIDER=package \
        -Dprotobuf_BUILD_PROTOC_BINARIES=OFF

    cmake --build "windows-build-deps/protobuf_build" --parallel 8 --target install

    touch "${winprefix}/protobuf_installed"
fi

cmake -S . -B mingw-build \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE \
    -DCMAKE_INSTALL_PREFIX="$(realpath -- reincarnation)" \
    -DCMAKE_TOOLCHAIN_FILE="$(realpath -- toolchain-windows-x86_64.txt)" \
    -DCMAKE_FIND_ROOT_PATH="$(realpath -- windows-build-root-path)" \
    -DBoost_INCLUDE_DIR="$(realpath -- "windows-build-deps/${windows_boost_version}")" \
    -G "Ninja" \
    -DPKG_CONFIG_EXECUTABLE="${winprefix}/bin/x86_64-w64-mingw32-pkg-config" \
    -DBUILD_ASSET_PROCESSING=OFF

cmake --build mingw-build
cmake --install mingw-build --component GameAssembly --strip
cmake --install mingw-build --component GameServer --strip

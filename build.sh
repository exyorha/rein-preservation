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
ln -sf libGameAssembly.so graft/GameAssembly.so

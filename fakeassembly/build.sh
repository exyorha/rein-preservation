set -e
mkdir -p ../graft
cmake -S . -B build -G "Kate - Ninja" -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX="$(realpath -- ../graft)"
ln -sf build/compile_commands.json .
cmake --build build --target install
ln -sf libGameAssembly.so ../graft/GameAssembly.so

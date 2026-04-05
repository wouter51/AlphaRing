#!/bin/sh
# Fix case-sensitivity mismatches for cross-compiling on Linux.
# Windows (NTFS) is case-insensitive; these symlinks emulate that for known paths.
set -e

# 1. lib build-type directories: Release -> release, Debug -> debug
for dir in lib/*/lib; do
    [ -d "$dir/release" ] && [ ! -e "$dir/Release" ] && ln -sf release "$dir/Release"
    [ -d "$dir/debug" ]   && [ ! -e "$dir/Debug" ]   && ln -sf debug   "$dir/Debug"
done

# 2. minhook lib filename casing (CMakeLists.txt says libMinhook, file is libMinHook)
for d in release Release debug Debug; do
    p="lib/minhook/lib/$d"
    [ -d "$p" ] && [ ! -e "$p/libMinhook.x64.lib" ] && ln -sf libMinHook.x64.lib "$p/libMinhook.x64.lib"
done

# 3. Source directory casing: includes reference D3d11/ and Window/ but dirs are d3d11/ and window/
[ -d "src/render/d3d11" ]  && [ ! -e "src/render/D3d11" ]  && ln -sf d3d11  "src/render/D3d11"
[ -d "src/render/window" ] && [ ! -e "src/render/Window" ] && ln -sf window "src/render/Window"

echo "Case-sensitivity symlinks created."

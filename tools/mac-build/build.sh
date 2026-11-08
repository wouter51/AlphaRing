#!/bin/bash
# Cross-build WTSAPI32.dll on macOS (Apple Silicon/Intel) with clang-cl + lld-link + an xwin SDK.
# One-time: brew install llvm lld xwin cmake
#           xwin --accept-license --arch x86_64 --variant desktop splat --output ~/.xwin/splat --include-debug-libs
set -euo pipefail
R="$(cd "$(dirname "$0")/../.." && pwd)"
export XWIN_DIR="${XWIN_DIR:-$HOME/.xwin/splat}"
cmake -S "$R" -B "$R/build" -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE="$R/tools/mac-build/clangcl.cmake" \
  -DCMAKE_BUILD_TYPE=Release -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDLL
cmake --build "$R/build" -j8
echo "Built: $R/build/WTSAPI32.dll"

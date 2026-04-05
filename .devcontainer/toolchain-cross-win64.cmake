# Cross-compilation toolchain: Linux -> Windows x64 using clang-cl + lld-link + xwin
# Used by the AlphaRing devcontainer to build WTSAPI32.dll

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR AMD64)

# --- Compilers and tools ---
set(CMAKE_C_COMPILER clang-cl)
set(CMAKE_CXX_COMPILER clang-cl)
set(CMAKE_LINKER lld-link)
set(CMAKE_AR llvm-lib)
set(CMAKE_MT llvm-mt)
set(CMAKE_RC_COMPILER llvm-rc)

# Target triple
set(CMAKE_C_COMPILER_TARGET x86_64-pc-windows-msvc)
set(CMAKE_CXX_COMPILER_TARGET x86_64-pc-windows-msvc)

# xwin only ships release CRT — force /MD for all build types
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL" CACHE STRING "")

# --- xwin SDK/CRT paths ---
if(DEFINED ENV{XWIN_DIR})
    set(XWIN_DIR "$ENV{XWIN_DIR}" CACHE PATH "Path to xwin output directory")
else()
    set(XWIN_DIR "/xwin" CACHE PATH "Path to xwin output directory")
endif()

# Include paths: MSVC CRT + Windows SDK (ucrt, um, shared)
string(CONCAT _XWIN_CL_FLAGS
    "-imsvc ${XWIN_DIR}/crt/include "
    "-imsvc ${XWIN_DIR}/sdk/include/ucrt "
    "-imsvc ${XWIN_DIR}/sdk/include/um "
    "-imsvc ${XWIN_DIR}/sdk/include/shared "
    "/EHsc"
)
set(CMAKE_C_FLAGS_INIT "${_XWIN_CL_FLAGS}")
set(CMAKE_CXX_FLAGS_INIT "${_XWIN_CL_FLAGS}")

# Library paths: MSVC CRT + Windows SDK
string(CONCAT _XWIN_LINK_FLAGS
    "/LIBPATH:${XWIN_DIR}/crt/lib/x86_64 "
    "/LIBPATH:${XWIN_DIR}/sdk/lib/um/x86_64 "
    "/LIBPATH:${XWIN_DIR}/sdk/lib/ucrt/x86_64"
)
set(CMAKE_EXE_LINKER_FLAGS_INIT "${_XWIN_LINK_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "${_XWIN_LINK_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "${_XWIN_LINK_FLAGS}")

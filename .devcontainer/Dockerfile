FROM ubuntu:24.04

# Install LLVM 19 repo (xwin's latest MSVC STL requires Clang 19+)
RUN apt-get update && apt-get install -y --no-install-recommends \
    curl ca-certificates gnupg \
    && curl -fsSL https://apt.llvm.org/llvm-snapshot.gpg.key \
       > /etc/apt/trusted.gpg.d/llvm.asc \
    && echo "deb http://apt.llvm.org/noble/ llvm-toolchain-noble-19 main" \
       > /etc/apt/sources.list.d/llvm-19.list \
    && apt-get update && apt-get install -y --no-install-recommends \
    cmake \
    ninja-build \
    clang-19 \
    lld-19 \
    llvm-19 \
    git \
    && rm -rf /var/lib/apt/lists/*

# Create unversioned tool symlinks
# clang-cl is clang invoked by a different name (argv[0] detection)
RUN ln -sf /usr/bin/clang-19 /usr/bin/clang-cl \
    && for tool in clang clang++ lld-link llvm-lib llvm-rc llvm-mt llvm-ar; do \
        ln -sf "/usr/bin/${tool}-19" "/usr/bin/$tool"; \
    done

# Install xwin to download Windows SDK and MSVC CRT
ARG XWIN_VERSION=0.6.5
RUN curl -fsSL "https://github.com/Jake-Shadle/xwin/releases/download/${XWIN_VERSION}/xwin-${XWIN_VERSION}-x86_64-unknown-linux-musl.tar.gz" \
    | tar xz --strip-components=1 -C /usr/local/bin "xwin-${XWIN_VERSION}-x86_64-unknown-linux-musl/xwin"

# Download Windows SDK and MSVC CRT (x64 only)
RUN xwin --accept-license --arch x86_64 splat --output /xwin \
    && rm -rf /root/.xwin-cache

# Create case-variant symlinks for .lib files.
# Linux is case-sensitive but Windows SDK headers reference libs with varying casing
# (e.g., "Version.lib" vs "version.lib"). This ensures both forms resolve.
RUN find /xwin -name '*.lib' | while read -r lib; do \
        dir=$(dirname "$lib"); \
        base=$(basename "$lib"); \
        lower=$(echo "$base" | tr '[:upper:]' '[:lower:]'); \
        title=$(echo "$lower" | sed 's/^\(.\)/\U\1/'); \
        [ -e "$dir/$lower" ] || ln -sf "$base" "$dir/$lower"; \
        [ -e "$dir/$title" ] || ln -sf "$base" "$dir/$title"; \
    done; exit 0

# Create case-variant symlinks for commonly-mismatched CRT/SDK headers
# (e.g., #include "String.h" resolves to <string.h> on case-insensitive Windows)
RUN [ ! -e /xwin/sdk/include/ucrt/String.h ] && ln -sf string.h /xwin/sdk/include/ucrt/String.h; exit 0

ENV XWIN_DIR=/xwin

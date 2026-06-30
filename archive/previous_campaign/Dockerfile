# Pokemon Colosseum decomp build container.
#
# Bakes the mwcc compiler bundle, objdiff-cli, decomp-toolkit, and Python build
# scripts into one image so the local toolchain can't be deleted by stray agents
# or cleanups (we lost ours twice during a long session).
#
# Build:
#   docker build -t pkmn-colosseum-build .
#
# Run (mount source as read-write, build artifacts isolated in /work/build):
#   docker run --rm -v "$(pwd)":/work pkmn-colosseum-build \
#     python3 tools/compile_check.py src/game/gs_title.c
#
# Or interactive:
#   docker run --rm -it -v "$(pwd)":/work pkmn-colosseum-build bash

FROM debian:bookworm-slim

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
        python3 python3-pip python3-venv \
        ninja-build \
        unzip curl ca-certificates \
        git \
        build-essential \
    && rm -rf /var/lib/apt/lists/*

# wibo lets us run the Windows mwcc binaries on Linux without wine.
# See https://github.com/decompals/wibo
RUN curl -L -o /usr/local/bin/wibo \
        https://github.com/decompals/wibo/releases/download/1.0.3/wibo \
    && chmod +x /usr/local/bin/wibo

# CodeWarrior 1.0-2.5 compiler bundle, pinned to dtk-template's 2025-11 release.
# Bundle is freely redistributable per the GameCube decomp community.
RUN mkdir -p /opt/mwcc \
    && curl -L -o /tmp/compilers.zip \
        https://files.decomp.dev/compilers_20251118.zip \
    && unzip -q /tmp/compilers.zip -d /opt/mwcc \
    && rm /tmp/compilers.zip \
    && ls /opt/mwcc/GC/1.3/mwcceppc.exe

# objdiff-cli for binary match measurement.
RUN curl -L -o /usr/local/bin/objdiff-cli \
        https://github.com/encounter/objdiff/releases/download/v3.6.1/objdiff-cli-linux-x86_64 \
    && chmod +x /usr/local/bin/objdiff-cli

WORKDIR /work

# The container expects the project repo mounted at /work. The build scripts
# look for tools/mwcc_compiler/* inside the repo, so symlink to /opt/mwcc.
# Symlink is created at container start time so it survives volume remounts.
COPY docker/entrypoint.sh /usr/local/bin/entrypoint.sh
RUN chmod +x /usr/local/bin/entrypoint.sh

ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
CMD ["bash"]

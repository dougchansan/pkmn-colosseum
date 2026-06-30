#!/usr/bin/env bash
# Container entrypoint: wire the baked-in toolchain into the mounted repo.
#
# The mwcc bundle and objdiff-cli live in the image at /opt/mwcc and
# /usr/local/bin so they survive any cleanup an agent might do inside /work.
# We expose them at the paths the project's Python scripts already expect.
set -eu

cd /work

# Symlink mwcc bundle into the project layout the scripts expect.
# Force-replace any stale links/files at the path.
mkdir -p tools/mwcc_compiler
if [ ! -e tools/mwcc_compiler/GC ] || [ -L tools/mwcc_compiler/GC ]; then
    ln -snf /opt/mwcc/GC tools/mwcc_compiler/GC
fi

# objdiff-cli is on PATH globally; also link into tools/ for scripts that
# reference the local path. Note: name kept as `.exe` because the existing
# Python scripts hard-code that path.
if [ ! -e tools/objdiff-cli.exe ] || [ -L tools/objdiff-cli.exe ]; then
    ln -snf /usr/local/bin/objdiff-cli tools/objdiff-cli.exe
fi

# wibo wrapper for the Windows mwcc binaries. The compile_check.py script
# invokes mwcceppc.exe directly; on Linux we need wibo to load it.
# Set the env var the build system uses (or fall back to running the binary
# directly if it's been wrapped elsewhere).
export WIBO=${WIBO:-/usr/local/bin/wibo}

# Hand off to whatever the user asked for.
exec "$@"

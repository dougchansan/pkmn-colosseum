# Containerized build for Pokemon Colosseum decomp

The mwcc compiler bundle, objdiff-cli, wibo, and Python build tools are baked
into a Docker image so they survive accidental deletion (we lost the local
copy twice in one session to stray agent cleanup commands).

## Build the image

```bash
docker build -t pkmn-colosseum-build .
```

The image bakes:
- CodeWarrior `mwcc` GCN compilers 1.0, 1.1, 1.1p1, 1.2.5, 1.2.5n, 1.3, 1.3.2, 2.0, 2.5
- `objdiff-cli` v3.6.1 (linux-x86_64)
- `wibo` 1.0.3 (Win32 wrapper for Linux)
- Python 3, ninja, git, build-essential

## Use the image

Mount your repo at `/work` and the entrypoint will symlink the baked-in
toolchain into the locations the existing Python scripts expect:

```bash
# One-off compile + measure:
docker run --rm -v "$(pwd)":/work pkmn-colosseum-build \
    python3 tools/compile_check.py src/game/gs_title.c

# Match scan:
docker run --rm -v "$(pwd)":/work pkmn-colosseum-build \
    python3 tools/match_scan.py fn_80022E54

# Interactive shell:
docker run --rm -it -v "$(pwd)":/work pkmn-colosseum-build bash
```

## Why this exists

`tools/mwcc_compiler/` is in `.gitignore` and stored as loose binaries.
During the multi-agent grind sessions, parallel agents in worktrees
occasionally clobbered the parent's mwcc directory. With the container,
the *image* holds the compiler at `/opt/mwcc`, and the repo gets a
symlink. Wipe `tools/mwcc_compiler/` all you want; restart the container
and the symlink is restored.

## CI

Same image is used by the GitHub Actions workflow (`.github/workflows/build.yml`)
so the CI build is byte-identical to local container builds.

## Updating the bundled toolchain

- mwcc bundle: bump `compilers_20251118.zip` URL in `Dockerfile`. Bundles
  live at `https://files.decomp.dev/compilers_YYYYMMDD.zip`.
- objdiff-cli: bump the tag in `Dockerfile` (latest tags at
  https://github.com/encounter/objdiff/releases).
- wibo: bump the tag in `Dockerfile` (latest at
  https://github.com/decompals/wibo/releases).

After bumping, rebuild and push to ghcr if you've wired that up.

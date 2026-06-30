# Pokémon Colosseum Decompilation — Setup Guide

## Prerequisites

| Tool | Required | Purpose |
|------|----------|---------|
| Python 3.10+ | Yes | Build system, scripts |
| Git | Yes | Version control |
| Rust/Cargo | Yes* | Building decomp-toolkit from source |
| Ninja | Yes | Build execution |
| Dolphin Emulator | Yes | RVZ → ISO conversion, DOL extraction |
| mwcceppc | Yes | MetroWerks compiler (matching builds) |
| Ghidra | Recommended | Static analysis / reverse engineering |
| objdiff | Recommended | Side-by-side assembly comparison |

*Or download pre-built dtk binaries from GitHub releases.

## Step 1 — Install Rust

```bash
# Windows (via rustup-init.exe)
# Download from https://rustup.rs/

# Linux/macOS
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
```

## Step 2 — Install decomp-toolkit (dtk)

### Option A: From Source (recommended)
```bash
cargo install decomp-toolkit
```

### Option B: Pre-built Binary
Download from https://github.com/encounter/decomp-toolkit/releases/latest
and place the `dtk` binary in `tools/`.

## Step 3 — Install Ninja

```bash
pip install ninja
```

## Step 4 — Install Dolphin Emulator

Download from https://dolphin-emu.org/download/

Ensure `DolphinTool` is on your PATH, or note the install location.

## Step 5 — Convert RVZ to ISO

The disc image must be in ISO format for dtk to process it.

```bash
# Using DolphinTool CLI
DolphinTool convert -i "Pokemon Colosseum (USA).rvz" -o orig/GC6E01/game.iso -f iso

# Or use the helper script
python tools/convert_rvz.py "path/to/Pokemon Colosseum (USA).rvz"
```

Alternatively, use Dolphin GUI: **Tools → Convert File**.

## Step 6 — Extract the DOL

```bash
# Extract start.dol and system files from the ISO
dtk dol extract orig/GC6E01/game.iso orig/GC6E01/

# Verify the DOL
dtk dol info orig/GC6E01/start.dol
```

Compute and record the SHA-1 hash:
```bash
sha1sum orig/GC6E01/start.dol
```

## Step 7 — Obtain MetroWerks Compiler

The `mwcceppc.exe` compiler from CodeWarrior for Embedded PowerPC is required to
produce matching builds. Place it in `tools/mwcc_compiler/`.

On Linux, install [wibo](https://github.com/decompals/wibo) to run the Windows binary:
```bash
cargo install wibo
```

## Step 8 — Initial Split

```bash
# Analyze DOL segments
dtk dol info orig/GC6E01/start.dol

# Generate initial splits
dtk dol split config/GC6E01/config.yml --out build/GC6E01
```

## Step 9 — Configure and Build

```bash
python configure.py
ninja
```

## Step 10 — Verify

The build should produce a byte-identical DOL:
```bash
sha1sum build/GC6E01/start.dol   # Must match original
```

## Recommended Additional Tools

- **Ghidra** with PowerPC/Gekko language: https://ghidra-sre.org/
- **objdiff**: `cargo install objdiff-cli` or download from https://github.com/encounter/objdiff/releases
- **decomp.me**: https://decomp.me/ (web-based scratch pad for matching functions)

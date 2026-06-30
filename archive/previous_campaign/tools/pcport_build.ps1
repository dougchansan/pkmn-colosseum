# pcport_build.ps1 — one-command bring-up of the Pokemon Colosseum PC port (host build).
#
# Verified working 2026-05-31 on this machine:
#   - CMake 4.3.2          C:\Program Files\CMake\bin\cmake.exe
#   - MSVC x86 (VS18)      generator "Visual Studio 18 2026", -A Win32 (32-bit; GameCube ptr model)
#   - Clang 22             C:\Program Files\LLVM\bin\clang.exe (for standalone probes)
#   - GLAD                 generated via `pip install glad` (gl 3.3 core) -> third_party/glad
#   - GLFW                 fetched by CMake FetchContent (needs git + network)
#   - SDL2                 OPTIONAL (audio/pad shims); not required for the render path
#
# Usage:  powershell -File tools\pcport_build.ps1            # configure + build pcport_bootstrap
#         powershell -File tools\pcport_build.ps1 -Probe     # also build+run the resolver probe
param([switch]$Probe, [switch]$Reconfigure)

$ErrorActionPreference = "Stop"
$root  = Split-Path -Parent $PSScriptRoot
Set-Location $root
$cmake = "C:\Program Files\CMake\bin\cmake.exe"
$clang = "C:\Program Files\LLVM\bin\clang.exe"
$git   = "C:\Program Files\Git\cmd\git.exe"
$env:PATH = (Split-Path $git) + ";" + $env:PATH    # FetchContent needs git on PATH

# --- GLAD (gl 3.3 core) ---
if (-not (Test-Path "third_party/glad/src/glad.c")) {
    Write-Host "[pcport] generating GLAD (gl 3.3 core)..."
    python -m pip install --quiet glad
    python -m glad --profile core --out-path third_party/glad --api "gl=3.3" --generator c
}

# --- configure (MSVC x86) ---
if ($Reconfigure -or -not (Test-Path "build_pc/CMakeCache.txt")) {
    Write-Host "[pcport] configuring (VS18 / Win32 / PCPORT=ON)..."
    & $cmake -S . -B build_pc -G "Visual Studio 18 2026" -A Win32 -DPCPORT=ON
}

# --- build ---
Write-Host "[pcport] building pcport_bootstrap..."
& $cmake --build build_pc --target pcport_bootstrap --config Debug
# NOTE: linking the full game still has unresolved fn_/lbl_ symbols (TUs not yet
# added to the CMake target). See docs/pc_port_roadmap.md (full-game-link task).

# --- optional: standalone resolver probe (no GL link needed) ---
if ($Probe) {
    Write-Host "[pcport] building + running the resolver probe (native x86)..."
    & $clang -m32 -O1 -D_CRT_SECURE_NO_WARNINGS -w tools/pcport_probe/resolver_probe.c -o build_pc/resolver_probe.exe
    & build_pc/resolver_probe.exe orig/GC6E01/disc/files/topmenu.fsys menu_bg00
}

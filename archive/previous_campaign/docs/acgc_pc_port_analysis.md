# Animal Crossing GC PC Port -- Analysis and Lessons for Colosseum

**Source:** https://github.com/flyngmt/ACGC-PC-Port
**Date:** 2026-04-09
**Purpose:** Extract actionable patterns from a successful GCN decomp-to-PC-port project

---

## Overview

The ACGC PC Port is a native x86 PC port of Animal Crossing (GameCube, GAFE01)
built on top of [ACreTeam's ac-decomp](https://github.com/ACreTeam/ac-decomp).
The decomped C code runs natively on PC with a custom translation layer replacing
the GameCube's GX graphics API with OpenGL 3.3.

Key stats:
- Language: C (95.9%), C++ (2.3%), Python (1.6%), Assembly (0.1%)
- Target: 32-bit x86 (MinGW32)
- Build: CMake + GCC via MSYS2
- Graphics: OpenGL 3.3 Core Profile
- Platform: SDL2 (windowing, input, gamepad)

---

## 1. Architecture: Thin Shim, Not a Rewrite

Their core strategy is **keep the decomped C code untouched** and only replace
the hardware abstraction layer (GX -> OpenGL 3.3, OS -> SDL2). Game logic is
unmodified -- only the platform boundary changes.

### What this means for Colosseum

Our `docs/pc_port_design.md` already plans the same approach:
- `src/dolphin/` -> SDL2/OpenGL shims
- `src/hsd/` -> calls into our GX shims (no HSD changes needed)
- `src/game/` -> completely untouched

**Validated:** This architecture works in production. No need to consider
alternative approaches (e.g., reimplementing HSD directly in OpenGL).

---

## 2. Asset Handling: Read the Disc Image at Runtime

ACGC reads all assets directly from the disc image (ISO/GCM/CISO) at startup.
No asset extraction pipeline. Users drop their disc image in a `rom/` folder.

Benefits:
- No reverse-engineering of archive formats (FSYS in our case)
- No asset conversion tooling
- No divergence from original file access patterns
- Legal simplicity (users provide their own disc)

### What this means for Colosseum

Our `src/dolphin/dvd/` replacement should map `DVDRead`/`DVDReadAsync` calls
to file offsets within the ISO. The FSYS archive layer (`src/game/fsys/`) sits
above DVD and would work unchanged.

```
Original:  Game -> FSYS -> DVDRead -> GCN disc hardware
PC port:   Game -> FSYS -> DVDRead -> fread() from ISO file
```

This eliminates months of asset extraction tooling. **Highest-value pattern
to adopt.**

---

## 3. Build System: CMake + MinGW32 (32-bit)

They target **i686 (32-bit x86)** via MSYS2/MinGW32 with CMake.

Why 32-bit matters:
- Original game code assumes 32-bit pointers throughout
- Struct layouts, casts, and pointer arithmetic all assume `sizeof(void*) == 4`
- 64-bit would require auditing every pointer cast in ~8,600 functions

### What this means for Colosseum

Our PC port build (separate from the CW decomp build) should:
- Use CMake + GCC/MinGW targeting i686
- Keep 32-bit to avoid pointer-width bugs
- Maintain the CW build for matching verification alongside the CMake build

We already have a `pcport/` directory with CMakeLists.txt scaffolding.

---

## 4. Phase 3 Is the Hard Gate

The ACGC port works because **ac-decomp is a complete C decompilation** with
zero asm wrappers. Every function is portable C.

### What this means for Colosseum

Our Phase 3 (replacing ~940 remaining `#if 1` asm wrappers) is the critical
prerequisite. Every `nofralloc` + raw PPC instruction block is a hard blocker
for portability.

Current state (2026-04-09):
- 77.8% functions with source (6,690 / 8,603)
- ~940 asm wrappers remain across ~40 files
- Largest concentrations: gs_field_world.c (95), effect_visual.c (89),
  ui_core.c (87), gs_thread.c (84)

**No port work should begin on a module until its asm wrappers are gone.**

---

## 5. GX -> OpenGL Translation Layer

ACGC wrote a custom GX-to-OpenGL 3.3 translation layer. This is the core
technical achievement of the port.

### What this means for Colosseum

We have two options for graphics:

| Approach | Replace at | Pros | Cons |
|----------|-----------|------|------|
| **A: GX shim** | GX API level | HSD code unchanged; proven by ACGC | Must implement full GX surface |
| **B: HSD shim** | HSD level | Skip GX entirely | Must understand all HSD internals |

**Recommendation: Option A (GX shim)**, matching ACGC's approach. Our HSD layer
calls GX functions; we implement those GX functions with OpenGL. This is already
the plan in `pc_port_design.md`.

Key GX subsystems to translate (from our existing analysis):
- TEV combiners -> GLSL fragment shaders
- Vertex formats -> VAOs/VBOs
- Display lists -> pre-built VBOs
- Texture formats (CMPR/I4/I8/IA4/IA8/RGB565/RGB5A3/RGBA8) -> GL textures
- XF (transform) -> vertex shader uniforms
- EFB copies -> FBO render targets

**Action:** Study ACGC's GX->OpenGL implementation once Phase 3 is further along.

---

## 6. SDL2 as the Platform Layer

ACGC uses SDL2 for:
- Windowing and OpenGL context creation
- Keyboard input (customizable via `keybindings.ini`)
- Gamepad input (SDL2 GameController API with hotplug)
- Likely audio (SDL2_mixer or SDL2 audio)

### What this means for Colosseum

Our replacement mapping (already in `pc_port_design.md`):

| GCN Module | PC Replacement | SDL2 Component |
|------------|---------------|----------------|
| `src/dolphin/vi/` | SDL2 window + GL context | SDL_CreateWindow, SDL_GL_* |
| `src/dolphin/pad/` | SDL2 gamepad | SDL_GameController* |
| `src/dolphin/os/` | SDL2 threading + timers | SDL_Thread, SDL_GetTicks |
| `src/dolphin/dvd/` | stdio file I/O | fopen/fread on ISO |
| `src/dolphin/ai/` + JAudio2 | SDL2 audio | SDL_AudioStream |

---

## 7. Save Compatibility (GCI Format)

ACGC keeps the native GCI (GameCube Image) save format, stored in a `save/`
directory. Saves are fully interchangeable with Dolphin emulator.

### What this means for Colosseum

We should do the same:
- Read/write the same memory card format Colosseum uses
- Store in a `save/` directory
- Allow importing saves from Dolphin (`.gci` files)
- Our `src/dolphin/card/` replacement layer handles this

---

## 8. Debug Tools from Day One

ACGC ships useful debug entry points:
- `--model-viewer [index]` -- view any model (structures, NPCs, fish)
- `--time HOUR` -- override in-game time
- `--no-framelimit` -- uncap framerate
- `--verbose` -- diagnostic logging

### What this means for Colosseum

Build debug infrastructure early in the port:
- Model/texture viewer to validate GX->OpenGL before the full game runs
- Scene selector to jump to specific game states
- Frame stepping for rendering debugging
- Verbose GX call logging to trace rendering issues

---

## 9. AI Tooling Precedent

ACGC's README notes: "AI tools such as Claude were used in this project
(PC port code only)" -- the porting layer, not the original decomp.

This validates our workflow: use AI for the port translation layer and
decompilation work while maintaining byte-accurate matching against the
original binary.

---

## Priority Roadmap (Updated)

| Priority | Action | Gate |
|----------|--------|------|
| **Now** | Grind Phase 3 asm wrappers to zero | Hard prerequisite |
| **Near** | Design DVDRead -> ISO fread shim | Easiest port layer |
| **Near** | Study ACGC's GX->OpenGL source | Direct reference |
| **Later** | Set up CMake + MinGW32 build | Need enough C code |
| **Later** | Implement GX shim (TEV, VTX, DL) | Core port work |
| **Later** | SDL2 platform layer (VI, PAD, OS) | Parallel with GX |
| **Last** | Debug tools, settings, polish | After game runs |

---

## References

- ACGC PC Port: https://github.com/flyngmt/ACGC-PC-Port
- AC Decomp: https://github.com/ACreTeam/ac-decomp
- Our GX->OpenGL design: `docs/pc_port_design.md`
- Our Phase 3 plan: `docs/phase3_plan.md`
- Our port scaffolding: `pcport/CMakeLists.txt`

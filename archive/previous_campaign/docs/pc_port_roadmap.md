# Pokémon Colosseum → PC Port Roadmap

> **Status:** authoritative roadmap as of 2026-05-31. Supersedes the sequencing in the
> older `pc_port_design.md` Appendix A and the stale `matching_plan.md` / `phase3_plan.md`
> (see `status_audit.md`). Grounded in the 4-agent feasibility survey of `src/pcport/`,
> the 42 `docs/` PC-port notes, and `tools/decomp_work/progress2.py`.

---

## 0. The goal — and the pivot

**Goal:** a *playable native PC port* of Pokémon Colosseum (GPXE01), Ship-of-Harkinian
style — reimplement the GameCube platform layer (GX, OS, DVD, PAD, audio) as host shims
and run the game's **own decompiled C** natively on top.

**The pivot (the single most important reframe):**

> For a PC port, **byte-exact matching is irrelevant.** An x86 build compiles the same C
> with GCC/Clang/MSVC; the GameCube bytes don't matter. The port needs C that *behaves*
> correctly, not C that reproduces CodeWarrior's exact PowerPC output.

Consequences, in plain terms:

- The metric that matters is now **C-converted % (functional C), not byte-exact %.**
  Today: **~77–83% C-converted** / 55% ROM-reproducible / **40% byte-exact** (the 40% is
  what we *stop* optimizing for). See `tools/decomp_work/progress2.py` and `WALLS.md`.
- The **entire ~3,043-function "Equivalent" pool** (real, correct C that doesn't byte-match)
  **counts fully for the port.**
- **Every wall in `WALLS.md` (W1–W6: reg-alloc, scheduling, stmw threshold, the
  `-use_lmw_stmw` dead-end) is irrelevant to the port** — those only block byte-identical
  PowerPC output and evaporate on x86. We stop grinding them.
- **Lane selection flips:** we pick the next decomp target by *"what the port's call graph
  needs next,"* not by *"what's a winnable byte-match near-miss."*

This does **not** waste the matching work done so far: byte-matching is what *validated*
that the ~77% of C we have is faithful. We keep the byte-match build as a correctness
oracle; we just stop spending effort pushing the last ~20% to byte-identical.

---

## 1. Current state (ground truth, 2026-05-31)

**Architecture — confirmed Ship-of-Harkinian:** `src/pcport/` (~12k lines) reimplements the
platform layer; the game's real decompiled C is compiled natively and linked on top. Every
shim `.c` is guarded `#ifdef __MWERKS__ … #else` so the *same files* serve both the
CodeWarrior matching build (compiles to nothing) and a host compiler. `pcport_main.c`
declares the real game functions `extern` and calls them: `GSgfxInit`, `GSgfx_BeginFrame`,
`fn_800DAD10` (gs_render.c), `fn_801AA568` (hsd_pobj_disp.c). The CMake `pcport_bootstrap`
target already compiles real TUs (`gs_gfx.c`, `gs_render.c`, `hsd_pobj_disp.c`, `VI.c`,
`OSStateFlags.c`, `TRKUtil.c`) against the shim.

**What works today (verified by the `--real-*` smoke tiers):**
- A 640×480 GLFW/OpenGL window opens; real decompiled game code links and runs.
- **Real asset → game code → host GPU → visible pixels:** loads a real disc archive
  (`topmenu.fsys`), LZSS-decompresses it, parses the real big-endian HSD scene graph,
  translates a material / texture / camera / joint, and renders it through the game's
  **own** `fn_801AA568 → fn_800DAD10` draw path — confirmed by framebuffer pixel-diff.
- Real-content texture decode (CMPR/I8/RGBA8), FSYS member parsing + LZSS.

**What's stubbed / not done:**
- **No game loop / boot.** `main()` is a flag-dispatched *smoke-test harness*; with no flag
  it prints "No game code… started" and exits. No title/menu/state machine.
- **No input** — `pad_shim.c` `PADRead` returns zeroed controllers (SDL2 mapping is TODO).
- **No audio** — `audio_shim.c` `JAudio_Init` just returns TRUE.
- **No disc mount** — `DVDInit_PC` returns success without opening the ISO; assets are read
  as loose extracted `.fsys` files.
- **Graphics is fixed-function placeholder** — the implemented path is OpenGL 1.x
  (`glVertexPointer`/`glTexEnvi`/`glMatrixMode`), **not** the planned GL 3.3 core +
  TEV→GLSL shaders. `gx_tev.c`'s shader generator is entirely TODO (no
  `glCreateShader`/`glCompileShader` is ever called).

**Maturity signal (lines / TODO):** `real_content_host.c` 1704 (0 TODO — most finished),
`pcport_main.c` 4075 (0 — the harness), `gx_shim.c` 1433 (**36 TODO**), `gx_tev.c` (**12**),
`gx_texture.c` (**11**), `os_shim.c` (8), `pad_shim.c` (10), `dvd_shim.c` (7).

**The 17 `real_scene_slice` docs** trace the disciplined bring-up: headless exe → window →
game-owned GSgfx init/swap → first visible quad → real camera → real geometry/material/
texture/TEV → a generalizing `TObj`/`TExp` content classifier (slice 17 = latest). The
blueprint is **ACGC** (Animal Crossing GC PC port, `flyngmt/ACGC-PC-Port`).

---

## 2. Two tracks (they interleave)

- **Track D — Decomp (functional).** Convert the port's still-asm functions to *functionally
  correct* C (NOT byte-match). PowerPC asm cannot run on x86, so every `asm{#include .inc}`
  on the port's call graph must become real C, a hand reimplementation, or a shim.
- **Track P — Port host layer.** The shims, the HSD resolver, the shader backend, the
  platform breadth (input/audio/disc), and finally the engine boot loop.

They gate each other: a scene slice (Track P) surfaces the next asm function on its call
graph (Track D); finishing that function unblocks the next slice. The roadmap below
sequences them by *what unblocks the most*.

---

## 3. Prioritized critical path

| Phase | Deliverable | Why here / gating |
|---|---|---|
| **P0 ✅** | This roadmap + goal-pivot adoption | Locks the metric (C-converted) and lane-selection rule |
| **P1 →** | **Host-safe HSD archive → `HSD_*Desc` resolver** | The single sharpest blocker (`real_content_render_bridge.md`): loaders only consume *materialized* descriptors; `fn_80191F64` (archive parse) is still asm. Unblocks loading arbitrary real scenes instead of hand-translating each in `real_content_host.c`. |
| **P2** | **Next `--real-*` scene slice** | Continue the proven march; widen the shared `TObj`/`TExp` parser one evidence-backed texture-expression family at a time (slice 17's next candidate: `title.fsys:logo_demo`). Each slice surfaces the next Track-D target. |
| **P3** | **Real TEV → GLSL shader backend** | Replace the fixed-function placeholder. Required for correct materials, multi-texture, alpha-test (`discard`), fog, lighting. Dolphin-style hash-cached generated shaders per `pc_port_design.md`. |
| **P4** | **Platform breadth** | SDL2 GameController (input) + SDL2_mixer w/ ADPCM→PCM (audio) + real disc/FST mount (`DVDRead → fread(ISO)`). Makes a slice *interactive* and removes the loose-file hack. |
| **P5** | **First engine boot loop** | Integrate `main.c` / `gs_thread.c` to drive a real title/menu state instead of a one-shot smoke test. This is the "it's actually a game" milestone — deferred until P1–P3 give it enough render breadth + content resolution. |
| **P6 (interleaved)** | **Decomp still-asm port-critical clusters to functional C** | Driven by P2/P5 call graphs. Heaviest: `gs_worldmap.c` (~61% C, ~39% asm) and the HSD transform/draw leaves `hsd_cobj.c` (74%), `hsd_jobj.c` (79%), `hsd_mobj.c` (77%). Functional bar only — no walls. |
| **P7** | **Breadth + polish** | Remaining texture formats (I4/IA/CI/TLUT), lighting/fog fidelity, FBO/EFB-copy post, widescreen, framerate, GCI save conversion. |

---

## 4. Track D reprioritized for the port

The decomp side is no longer "drive every function to byte-match." It is:

1. **Only port-critical asm matters.** Of the ~815 still-active asm wrappers + ~580 stubs,
   the port cares only about those on its actual call graph (graphics/scene/HSD/boot).
   Build a **port-blocking-asm tracker**: walk the call graph from the port's entry points
   (`GSgfxInit`, `fn_800DAD10`, `fn_801AA568`, the boot chain) and flag which still-asm
   functions are reachable. Those are the worklist; everything else is deferrable.
2. **Functional bar, not byte-match.** These conversions need correct behavior only. That
   means Ghidra/m2c output + light cleanup is often *enough* — no pragma sweeps, no
   allocator inversion, no permuter. The `WALLS.md` walls do not apply.
3. **Per-file focus** (from the survey's REAL_C ratios): the port-relevant gaps are
   `gs_worldmap.c` (~39% asm) and the HSD transform/draw hot paths (`hsd_cobj`/`hsd_jobj`/
   `hsd_mobj` matrix + primitive-draw leaves). `gs_gfx`/`gs_model`/`hsd_pobj`/`hsd_tobj`/
   `hsd_render`/`hsd_tev` are already 100% C.
4. **Watch for Gekko paired-singles / quantized loads** in any FP-heavy asm — those need
   correct x86 FP semantics (the one genuinely fiddly part of GC→x86).

---

## 5. Tooling / process changes

- **`progress2.py` C-converted axis is now the port progress metric.** Report it, not the
  55% ROM number, as "how close to portable."
- **New tracker (P6 prerequisite):** a call-graph-reachability tool that lists still-asm
  functions reachable from the port entry points → the prioritized Track-D worklist.
- **Keep the `#ifdef __MWERKS__` dual-build discipline** so every converted file still
  serves the byte-match oracle *and* the host build.
- **Keep the byte-match build as a correctness oracle** for any function we *also* want to
  verify, but don't gate port work on it.
- **De-emphasize:** the permuter grind, allocator-inversion research (task #17), and
  byte-match wall lanes — low value for the port goal.

---

## 6. Risks / open questions

- **HSD resolver (P1):** `fn_80191F64` archive parse is asm; the host needs a clean
  `archive bytes → HSD_*Desc` path. Risk: the real parser has dependencies we must also
  port. Mitigation: decompile `fn_80191F64` functionally first (it's Track-D work anyway).
- **TEV fidelity (P3):** fixed-function GL can only approximate TEV; full correctness needs
  the generated-shader backend. Risk of a long tail of material edge cases.
- **Scale:** the `pcport_bootstrap` target compiles a *curated slice* of TUs, not the whole
  game (`GAME_SOURCES`/`HSD_SOURCES` still empty). Scaling to full-game link will surface
  ABI/header/endian portability work across ~8,600 functions (32-bit i686 build chosen to
  limit pointer-cast auditing).
- **Boot loop (P5):** the real boot chain (`main.c`/`gs_thread.c`) may pull in large
  still-asm subsystems; expect P5 to expand the P6 worklist.

---

## 7. Immediate next actions (agreed sequence)

1. **P0 — this doc.** ✅
2. **P1 — HSD archive→Desc resolver.** Next up: decompile `fn_80191F64` (functional) and/or
   build a host-safe `HSD_ArchiveParse`/`*LoadDesc` so real scenes load without
   hand-translation in `real_content_host.c`.
3. **P2 — next scene slice.** Following the established `--real-*` pattern, widen the
   `TObj`/`TExp` classifier with the next evidence-backed family.

Then iterate P3→P5, pulling Track-D conversions (P6) as each slice/boot step demands.

---

*Sources: `docs/pc_port_design.md`, `docs/acgc_pc_port_analysis.md`,
`docs/real_content_render_bridge.md`, `docs/real_scene_slice_2.md`..`_17.md`,
`docs/status_audit.md`; `src/pcport/*`; `WALLS.md`; `tools/decomp_work/progress2.py`.*

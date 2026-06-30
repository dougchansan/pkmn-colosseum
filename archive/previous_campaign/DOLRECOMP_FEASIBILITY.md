# DolRecomp for the Pokémon Colosseum PC port — feasibility note

**For:** the PC-port track (`src/pcport`).
**From:** the decomp/RL session, 2026-06-24.
**Re:** [github.com/ExpansionPak/DolRecomp](https://github.com/ExpansionPak/DolRecomp) — a static GC/Wii recompiler released 2026-06-10.

## TL;DR
DolRecomp can automate the **CPU-translation half** of a PC port — the GameCube analogue of N64Recomp/Zelda64Recomp. But it's two weeks old, ships **CPU-only with no runtime**, and the runtime (GX, DSP, OS, PAD, FS) is 100% on us — which is the *bulk* of any GC port. Worth a bounded spike on the port track; **does not** help the matching decomp or the RL flywheel.

## What it actually is
- **Static recompiler:** reads DOL/REL/RPX, decodes PowerPC (Gekko/Broadway/Espresso, 236 opcodes), emits **split C that reproduces the CPU behavior** and compiles on a PC.
- README, verbatim: *"The current project is CPU only. You will need to supply your own runtime… The generated C is a compile target only, no runtime."*
- Features relevant to us: **DOL + REL module handling** (single + folders, resolves inter-module imports), `-jN` split codegen, built-in GameCube ISO extraction, and a **function-address dispatch** hook that can "hand known function addresses to host patches before entering compiled code" (the integration seam for HW/OS interception).
- New (2026-06-10), **GPL-3.0**, C, ~24 stars. No demonstrated fully-running ported game in the README.

## Why it matters for the port
The strategic win is **decoupling "playable on PC" from "finish the matching decomp"** (which is a multi-year effort, function-by-function). DolRecomp gives a runnable CPU translation of the *entire* binary today, regardless of how much source we've recovered. That's the whole Zelda64Recomp value proposition, and DolRecomp is purpose-built for GameCube (DOL + REL), which fits Colosseum directly (main DOL + its REL modules).

## The cost — be honest about it
CPU translation is roughly the **easy 20%** of a port. With "no runtime," we still hand-write all of:
- **GX graphics** → a modern backend (the single biggest piece).
- **DSP audio**, **OS/threads** (OSThread, alarms, message queues), **DVD/FS**, **PAD/controller**, **memory card**.
The function-address dispatch is where this plugs in: intercept GX/OS/DSP entry points and route to our runtime instead of the recompiled code. So DolRecomp removes the CPU-porting labor but not the platform-emulation labor.

## How it sits next to the current `src/pcport` approach
You know the current approach better than I do — this note defers to that. The key framing: DolRecomp is a **different strategy**, not a drop-in. If `src/pcport` is a hand-port driven by the matching decomp, the two can **coexist**: DolRecomp static-recomps the bulk CPU for immediate runnability, while hand-decompiled source replaces hot/critical paths over time (recomp ports often do exactly this). The decision is whether a recomp-first port gets to "playable" materially faster than the current path.

## Recommended spike (bounded, ~1 day)
1. Build DolRecomp (`cmake -S . -B build && cmake --build build --config Release`).
2. `dolrecomp --gamecube <colosseum main.dol> build` (+ the REL modules in folder mode). Confirm the generated C **compiles**.
3. Stub a **minimal runtime**: flat RAM, SDA bases (r2/r13), a handful of OS calls, and the dispatch hook. Drive the boot path and see how far the CPU gets before it needs a real GX/OS service.
4. From where it stalls, **scope the GX/DSP/OS runtime effort** — that estimate is the real go/no-go for a recomp-first port.

## Caveats
- **GPL-3.0** copyleft → anything derived is GPL. Check this against the port's intended licensing/distribution.
- **Maturity:** 2 weeks old; the decoder may have gaps (README flags Espresso as needing work — less relevant for GC/Gekko), and no proven full game yet. Expect to file/fix bugs.
- **Performance** is unmeasured here; recomp ports generally run well, but that's not a given for this tool yet.

## Verdict
**Worth a spike for the port track specifically.** It could shortcut the CPU-porting work and make a playable Colosseum-on-PC far less dependent on completing the decomp — at the price of writing the whole runtime ourselves and accepting an immature, GPL-3.0 dependency. It is **irrelevant to the byte-matching decomp and the RL flywheel**, which target source recovery, not runnability.

*(Separately, the decomp side is prototyping one narrow use of DolRecomp for itself: a behavioral-equivalence oracle to machine-verify the `equivalent.txt` walled tier. That's an internal experiment, independent of the port decision above.)*

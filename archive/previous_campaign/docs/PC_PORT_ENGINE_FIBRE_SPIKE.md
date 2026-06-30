# P-A Spike — Host Cooperative-Fibre Runtime (engine-hosting keystone)

> **Status:** spike COMPLETE + PASSING, 2026-06-02. Implements the **P-A** row of
> `docs/PC_PORT_TO_PLAYABLE.md` §4 (the strategic fork's option **(B) authentic
> engine-hosting**). PC-port work only — functional C on host fibres, no
> byte-match decomp. All new code in `src/pcport/**`; the read-only engine
> sources (`src/game/*`, `src/dolphin/*`) were studied, never edited.

---

## 0. One-line outcome

A native **Win32-fibre cooperative runtime** that reimplements the *semantics* of
the engine's per-frame vsync-yield (`fn_800F0308`) and thread context-switch, and
**ticks real frames**: a thread shaped exactly like the real title thread
`gs_title.c fn_8002058C` (`init(); for(;;) yield();`) runs natively, round-tripping
to a host **present** fibre each frame. **The mechanism that gates all gameplay
(P-A) is de-risked and proven.**

---

## 1. Why a host fibre runtime (the problem)

The GameCube build runs the engine as a **cooperative fibre scheduler**. Threads
switch with PowerPC assembly that hand-saves/restores the full register file
(GPR/FPR/SP/LR) into a per-thread context block:

- **`fn_800F0308`** (`src/game/gs_texture.c`, asm) — the per-frame **vsync-yield**
  the whole engine blocks on. It saves the running thread's context (via
  `fn_800F015C`/`fn_800F01F0`), then loads the scheduler-selected *next* context
  (via `fn_800F0030`/`fn_800F00C0`) and `blr`s into it. The real title thread
  `fn_8002058C` is literally `lbl=0; fn_801EF644(-1); lbl=1; for(;;) fn_800F0308();`.
- **`src/dolphin/os/OSThread.c`** — the lower OSThread layer (`OSCreateThread`,
  `__OSThreadSwitch`, `OSLoadContext`/`OSSaveContext`), also asm-origin for the
  context switch. `fn_800A263C` (thread/fibre create) is asm-active here.

That asm **cannot run on x86**. To host the engine's own loop we must reimplement
the context-switch *semantics* — "suspend this coroutine, resume that one,
preserving its entire call stack" — natively. On x86 the clean primitive is
**Win32 Fibers** (`ConvertThreadToFiber`/`CreateFiber`/`SwitchToFiber`): each fibre
owns its stack + CPU state and a switch is explicit and non-preemptive — exactly a
cooperative coroutine.

**Key insight that makes this tractable:** on PowerPC the engine hand-marshalled
the register file because a coroutine switch *is* "swap SP/LR/GPRs." On x86 with
fibres, `SwitchToFiber` preserves the whole CPU + stack automatically, so we
**only reimplement scheduling (who runs next), not register marshalling.** The
`GSThreadCtx` GPR/FPR fields are irrelevant on host; each engine thread-context
maps 1:1 to a host fibre.

---

## 2. What was delivered (all in `src/pcport/`)

| File | Role |
|---|---|
| `os_thread_host.h/.c` | Generic, engine-agnostic **fibre primitive layer** over Win32 Fibers: `HostFibre_InitMain` (convert the host thread, float-state preserved), `HostFibre_Create`, `HostFibre_SwitchTo`, `HostFibre_Current`, resume accounting. POSIX `ucontext` / thread+condvar fallback is documented for portability (reports `HostFibre_Available()==0` off-Windows). |
| `engine_host.h/.c` | **Host reimplementation of the engine's cooperative scheduler** on that layer. `EngineHost_CreateThread` ≙ `OSCreateThread`/`fn_800F07A8` (→ `CreateFiber`); `EngineHost_VsyncYield` ≙ **`fn_800F0308`** (→ switch back to the host/scheduler fibre, returns next frame); `EngineHost_Run` is the host present-driver loop (round-robins every engine thread's per-frame slice, then presents). |
| `engine_spike.c` | The spike entry points + an engine thread shaped exactly like `fn_8002058C`. `RunFibreSelfTest()` (headless) and `RunEngineSpike(window)` (windowed). |
| `pcport_main.c` (minimal wiring) | `--fibre-test` (headless self-test, early return like `--thp-smoke`) and `--engine` (windowed present loop). The existing `--menu` front-end is untouched and still works. |
| `tools/pcport_link.py` | Added the three new TUs to the `BOOT` list. |

### The host↔engine model (single OS thread, cooperative)
```
   [host / present fibre]  <—— EngineHost_VsyncYield (== fn_800F0308) ——  [engine thread fibre]
            |  resume next frame (HostFibre_SwitchTo) ——————————————————————→  |
   VIWaitForRetrace_PC → clear → GSgfxSwapBuffers           init(); for(;;){ work; yield(); }
```
The host fibre is the scheduler/present driver. Each engine thread runs its
per-frame slice then yields; once all registered threads have yielded, the host
presents one frame and resumes them. This is the faithful shape of the real
engine's `for(;;) fn_800F0308()` thread loops driven by the GSgfx swap callback at
vsync.

---

## 3. Proof it works (reproducible)

Build: `python tools/pcport_link.py` → *compiled 41 objects; 0 failed; LINKED OK*.

**Headless mechanism self-test** (no GL):
```
build_pc\pcport_bootstrap.exe --fibre-test        # PCPORT_FIBRE_FRAMES=N (default 10)
```
Output (verbatim, trimmed):
```
[engine_host] cooperative-fibre scheduler initialised (host fibre live)
[spike] engine thread 'title': init slice running
[spike] frame 0 presented (host fibre); t0.work=1 t1.work=0
...
[spike] part A: ticked 10 frames; title.initDone=1 title.work=10
[spike] part A PASS: round-trip count matches frame count
[spike] part B: ticked 10 more frames; t0.work=20 t1.work=10
[spike] part B PASS: both engine threads sliced once per frame
[spike] === self-test PASS ===   (exit 0)
```
- **Part A** — one engine thread shaped like `fn_8002058C`: its init slice runs,
  then it yields once per frame for exactly N frames; the work-tick counter equals
  the frame count (the round-trip is 1:1 with vsync).
- **Part B** — a second engine thread (models `OSCreateThread` spawning a thread
  alongside the main idle loop): **both** threads are sliced exactly once per frame
  (round-robin cooperative scheduling), thread 0 continuing its counter from part A.

**Windowed present loop** (real host present path):
```
PCPORT_ENGINE_FRAMES=120  build_pc\pcport_bootstrap.exe --engine
```
→ 120 frames presented through `VIWaitForRetrace_PC → clear → GSgfxSwapBuffers`
driven *from the host fibre after the engine thread yielded*; `engine.work=120`,
`initDone=1`, clean exit. `PCPORT_ENGINE_FRAMES` unset ⇒ runs until window close.

This satisfies the P-A milestone ("the engine loop advances and you can prove it
TICKS… even 'runs N frames then cleanly yield/exit' proves P-A").

---

## 4. The real-engine boot cascade (assessment — the important finding)

The spike proves the **mechanism**. Booting the *actual* engine (`main.c` →
`GameInit` → `fn_800F07A8` → `for(;;) fn_800FE7A0()`) onto this runtime is **P-B**,
and the cascade is deeper than "link a few TUs":

1. **You do NOT link `gs_thread.c`.** It is a 153 KB grab-bag TU with **86
   still-asm functions** (`gs_task.c`: 42, `gs_texture.c`: 13, incl. the 6 context
   primitives + `fn_800F0308`/`fn_800F028C`). The C-active scheduler functions
   (`GStaskInit`/`GSthreadCreate`/`GStaskCreate`/`GStaskRun`/`GSthreadInit`) are
   *islands* in that asm sea — the TU will not compile host-side as-is (asm blocks
   + missing `.inc` for the non-threading functions), and `pcport_gen.py` only
   flips HSD TUs.
2. **The C-active scheduler sits on more asm/heap.** `GSthreadCreate` allocates the
   stack + `GSThreadCtx` from the **GSmem allocator** (`fn_800E2*/E3*` family),
   calls `fn_800A263C` (OSThread asm thread-create), and inits the ctx via the asm
   `fn_800F015C`/`fn_800F01F0`. The scheduler is wired to GSgfx via the swap
   callback `fn_800FEBA0` (asm).
3. **`GameInit` touches ~60 hardware subsystems before threading** — GX FIFO
   (`0xCC008000`), VI registers, ARAM, DVD, DSP/sound — each needing a host shim or
   stub. (`pcport_link.py` already auto-stubs 187 residual asm-only symbols, but
   *functional* boot needs real shims on the path, not no-op stubs.)

**Therefore the viable P-B path is NOT "link the engine's scheduler" but
"host-reimplement its small C scheduler surface" — which is precisely what
`engine_host.c` is the foundation for.** `GSthreadCreate`/`GStaskCreate`/
`GStaskRun`/`GSthreadInit` are ~6 well-understood functions; mirror their priority
list + task table on `engine_host`'s fibre model (mapping each `GSThread` → a host
fibre and each `GSTask` → a callback entry), feeding host-`malloc` stacks instead
of GSmem handles. The engine's *game* threads (the title/field/battle loops) then
run on this host scheduler **without linking `gs_thread.c` at all** — they only
need their own TUs + the leaf engine code converted to functional C (Track D).

### Still-asm on the immediate boot/scheduler path (the P-B worklist seed)
- `fn_800F0308`, `fn_800F028C`, `fn_800F015C`, `fn_800F0030`, `fn_800F01F0`,
  `fn_800F00C0` (`gs_texture.c`) — **superseded host-side** by `EngineHost_VsyncYield`
  / `HostFibre_SwitchTo`; no PPC port needed, just the host mapping.
- `fn_800A263C` (`OSThread.c`) — thread-create; host-side ≙ `HostFibre_Create`.
- `fn_800FEBA0` (swap callback), the GSmem `fn_800E2*/E3*` family — needed only if
  you link the real scheduler; **the reimplement-the-surface path avoids them.**
- SI-poll input stand-ins `fn_800AB150`/`fn_800D0F44` (per the handoff) — still to
  be located + host-stubbed against `pad_shim` when the real input path is wired.

---

## 5. Recommended next step (P-B)

Grow `engine_host.c` from the spike's stand-in thread into a faithful host
**GSthread/GStask** reimplementation (priority-ordered thread list + task table,
matching `gs_thread.c`'s C surface), then drive **one real engine game thread**
(e.g. a host-built title/field loop TU) on it — input via the `pad_shim` SI
stand-ins, present via the existing path. That converts P-A's proven mechanism into
"the engine's own per-frame logic runs," which is the gate to the field/overworld
(P-C). Audio (P-F) and the TEV→GLSL backend (P-G) remain independent parallel lanes.

---

## 6b. P-B increment 1 — the host GStask/GSthread scheduler (2026-06-02)

Building on the P-A mechanism, P-B stands up the engine's **own** cooperative
scheduler, host-reimplemented (not linked) per the §4 cascade finding.

**`src/pcport/gs_sched_host.{h,c}`** — a faithful host reimplementation of
`gs_thread.c`'s scheduler surface, using the **real** structs/enums from
`include/game/gs_thread.h` (`GSTask` 0x18, `GSThread` 0x24, `GSTASK_FREE/ACTIVE/
DEFERRED`) so it is a drop-in for engine callers:
- **Task layer** (`GStaskInit`/`GStaskCreate`/`GStaskRun`) — reproduced *verbatim*
  from the original C (priority-sorted linked-list insert, 1-based task IDs,
  free-slot search over normal/deferred regions, the per-frame `GStaskRun` walk
  that calls every `ACTIVE && !paused` task's `func(taskId, param)`). GSmem handle
  allocation is replaced by static pools; everything else is identical. This is
  the engine's *dominant per-frame mechanism* and is pure C — no asm.
- **Thread layer** (`GSthreadInit`/`GSthreadCreate`) — each `GSthread` maps to a
  host fibre (`os_thread_host`); `GSthreadYield` is the host equivalent of the
  asm vsync-yield `fn_800F0308`; `GSthreadStepAll` resumes each thread's per-frame
  slice. The GSmem stack/ctx + asm `fn_800F015C` context-init are replaced by the
  fibre.
- Exports `fn_` aliases (`fn_800FE9B0`/`834`/`7A0`, `fn_800F07A8`) so that when
  real engine TUs are linked they bind to this host scheduler instead of the
  `pcport_link.py` auto-stubs. (`fn_800F09D8` is deliberately **not** aliased —
  the decomp annotations conflict on whether it is `GSthreadInit` or render-timing.)

**Proof:**
- `--sched-test` (headless) **PASSES**: priority-ordered task run `[2,3,1]` for
  creation-order priorities `30,10,20`; correct 1-based IDs; `DEFERRED` tasks
  skipped by `GStaskRun`; free-slot reuse on re-init; two fibre-backed `GSthread`s
  each sliced exactly once per `GSthreadStepAll` for 5 steps.
- `--engine-boot` (windowed, `src/pcport/engine_boot.c`) mirrors `main.c`'s
  `GameInit` structure — `GSthreadInit(4)` + `GStaskInit(16,4)` (= the real
  `fn_800FE9B0(0x10,0x4)`), three priority-ordered tasks (one driving the real
  host present path `VIWaitForRetrace_PC → clear → GSgfxSwapBuffers`, exactly as
  the real `TaskVBlank` does), and a main thread created like the real
  `fn_800F07A8(0,0x3E8,0x4000,1,1,GameMainLoop)`. The engine's own main loop
  `for(;;){ GSthreadStepAll(); GStaskRun(); }` ran **120 frames**: main-thread
  work counter `1→120` (1:1 with frames), 3 tasks fired each frame, real present
  path driven by the scheduler. Clean exit. `--menu`/`--fibre-test` unaffected.

**What this proves:** the engine's *own* cooperative scheduler model (task table +
thread fibres, semantically identical to `gs_thread.c`) now drives the host frame
loop, with real present code executing under it as a registered task.

**P-B increment 2 — real title scene rendered under the scheduler (DONE).** The
`--engine-boot` VBlank task now renders the **real** title scene graph through the
game's own draw bridge: `PCPort_EngineTitleSetup` loads `title.fsys:logo_demo`,
parses the HSD archive, resolves the scene camera + root joint (additive functions
in `pcport_main.c` reusing the same static helpers as `RunMenuScene`, which is left
untouched), and `PCPort_EngineTitleRenderFrame` runs `RenderJointTree → fn_800DAD10`
each frame. `PCPORT_BOOT_DUMP=<bmp>` (frame `PCPORT_BOOT_DUMP_FRAME`, default 80)
captures it: the dump shows the real desert/ruins title scene (sandstone walls,
sky band, ground plane) — **the same scene-graph render `RunMenuScene` produces, now
driven by the host GStask/GSthread scheduler's task** rather than a hand-coded loop.
Falls back to a flat clear if assets are unavailable. `--menu`/`--sched-test`/
`--fibre-test` unaffected.

**P-B increment 3 (next):** begin replacing the host-stub task bodies with the real
engine callbacks (`TaskVBlank → fn_80175F6C` world render, `TaskRetraceMain`,
`TaskPadRead`), converting their still-asm leaves to functional C (Track D) as the
call graph is walked. The deepest step — linking the real `main.c GameInit` with its
~60 hardware-subsystem inits — remains gated on the GX-FIFO/VI/ARAM/DVD/DSP shims.

## 6c. P-B increment 3 — the wall: real engine callbacks need Track-D (2026-06-02)

inc3's goal was to replace the host-stub task/thread bodies in `--engine-boot` with
the **real** engine callbacks (`TaskVBlank → fn_80175F6C` world render; the title
thread `fn_8002058C → fn_801EF644` title-init). A thorough investigation found this
is gated on Track-D functional decomp, for two compounding reasons:

**(1) The real callbacks live in TUs that don't compile host-side, or no-op without
booted data.**
- **Title thread** `gs_title.c fn_8002058C` is C-active and trivial
  (`fn_801EF644(-1); for(;;) fn_800F0308();`). Its real work is **`fn_801EF644`**
  (title attract-demo init) in `battle/battle_main.c`. **Discovery: `battle_main.c`
  is 100% C-active (0 asm wrappers)** — it is the most host-linkable real-engine TU,
  blocked only by **6 named conflicts** (`fn_80129280`, `fn_800D3088`, `fn_801659FC`,
  `fn_80165A20`, `fn_801EF634` — CW-tolerated extern redeclarations; and data label
  `lbl_80375CC8` `u8[]` vs `void*`), all fixable with the existing `pcport_gen.py`
  `FUNC_PROTO_KR`/`FUNC_STUB_DROP`/`EXTERN_UNIFY` machinery.
  *But even linked, `fn_801EF644` no-ops*: it loops over the demo table
  `lbl_803752A0` bounded by the count `lbl_80478D10`, both of which are auto-stubbed
  to **zero** on host (the real values come from the data sections the real
  `GameInit` would populate) → the count check breaks immediately. So linking it
  proves the function *executes* under our scheduler, but does nothing observable.
- **`TaskVBlank` render** `fn_80175F6C` (world/scene render) is still-asm and its TU
  is not linked; the surrounding `TaskVBlank` GX-state calls (`fn_80101B90`/
  `fn_80101D8C`/`fn_80101D5C`) live in **`gs_model.c`, which is not in the BOOT
  link** (auto-stubbed). And `fn_80175F6C` only renders if the world/scene system
  was booted by `GameMainLoop` (`fn_80179BEC` world init, etc.).
- **`gs_thread.c`/`gs_title.c`** are asm-heavy (86 / many asm fns) — not host-
  compilable as-is (the reason inc1 host-*reimplemented* the scheduler).

**(2) Real per-frame engine LOGIC needs the real data + subsystem boot**, which needs
the real `main.c GameInit` (~60 hardware-subsystem inits: GX-FIFO `0xCC008000`, VI,
ARAM, DVD, DSP) — the deepest gated step.

**What inc3 delivered (the enabler, safely):** `fn_800F0308` is now aliased host-side
to **`GSthreadYield`** (`gs_sched_host.c`) — so the moment a real engine thread body
is made host-linkable (Track D), its `for(;;) fn_800F0308()` loop yields through the
host fibre scheduler instead of the auto-stub. It is a guarded no-op when no host
scheduler fibre is live, so `--menu`/`--engine-boot`/`--sched-test` are unaffected
(verified). No real TU was forced into the green build for a no-op result.

**Track-D worklist for engine-hosting the title (ordered by tractability):**
1. **`battle_main.c`** — add the 6 conflict fixes to `pcport_gen.py`, link it
   (unlocks `fn_801EF644` + the battle system surface). Low effort; high unlock.
2. **Boot the title demo data** — populate `lbl_80478D10`/`lbl_803752A0` (or call the
   real registration path) so `fn_801EF644` drives a real attract demo, not a no-op.
3. **`gs_model.c`** + the **`fn_80175F6C` world-render TU** — make host-compilable
   (Track-D their asm leaves) so the real `TaskVBlank` render path runs.
4. **`main.c GameInit`** — host-shim the ~60 hardware inits (GX-FIFO/VI/ARAM/DVD/DSP)
   so the real boot chain executes end-to-end. The largest step.

inc1+inc2 already prove the scheduler + real *render* code run under host control;
inc3 establishes that real *game-mode logic* is a Track-D march, not a single wiring
step, and lays the `fn_800F0308` rail for it.

## 6d. Track-D #1+#2 — a real engine function runs under the scheduler (2026-06-02)

Executed the first two Track-D worklist items from §6c.

**#1 — `battle_main.c` is now host-linked.** It is 100% C-active; the 6 conflicts
were resolved in `pcport_gen.py` (no source edits — the generated copy only):
- `fn_801659FC`/`fn_80165A20` → `FUNC_PROTO_KR` (arg-type-only conflict; K&R `()` is
  compatible with the comment-bearing file-scope prototype + the 3-arg call sites).
- `fn_80129280` → `FUNC_PROTO_RETYPE` to `void* fn_80129280()` (both decls disagreed
  on return *and* args; K&R-args with one return unifies the 0-arg and 2-arg calls).
- `fn_801EF634` → `FUNC_STUB_DROP` (redundant `void ()` stub clashing with the real
  `u16 fn_801EF634(void)` definition that precedes it).
- `fn_800D3088` + `lbl_80375CC8` → `TEXT_FIXUPS` (their file-scope decls carry
  trailing comments, so the declaration regexes skip them; fixed textually —
  `fn_800D3088` void→u32 return, `lbl_80375CC8` `void*`→`u8[]` data-object form).
A new `GAME_GEN` list in `pcport_link.py` runs it through `pcport_gen` and compiles
it. Link converges (undefs 187→259, all auto-stubbed); `--menu`/`--sched-test`/
`--engine-boot` unaffected.

**#2 — the real `fn_801EF644` (title attract-demo init) runs under the host
scheduler.** With `PCPORT_REAL_TITLE_INIT=1`, `--engine-boot`'s main thread calls
the **real decompiled `fn_801EF644(-1)`** (mirroring the title thread `fn_8002058C`)
instead of the stand-in init. It executes and returns cleanly — **real engine code
running on the host GStask/GSthread cooperative scheduler, with its `fn_800F0308`
yields routed through `GSthreadYield`**. As predicted it is a no-op on host (its demo
table `lbl_803752A0` + count `lbl_80478D10` auto-stub to zero), so it returns at the
count check without entering the demo loop — but it proves the real function executes
under the scheduler without crashing, and that the whole `battle_main.c` surface is
host-callable.

**Next (Track-D #2 stretch / #3):** boot the demo-table data (`lbl_80478D10`/
`lbl_803752A0` — from the real `.data`/registration path) so `fn_801EF644` drives an
actual attract demo rather than no-op; then `gs_model.c` + the `fn_80175F6C`
world-render TU for the real `TaskVBlank`.

## 6e. Track-D foundational — 10 more engine TUs host-linked in parallel (2026-06-02)

A **10-agent workflow** grew the host-linked real-engine surface from 1 game TU
(`battle_main.c`) to **11**, by analyzing fully-C-active (0-asm) engine TUs in
parallel — each agent classified its TU's host-compile conflicts into the
`pcport_gen` mechanisms and verified a clean recompile, returning structured
fix-entries that were merged in one pass.

TUs added to `GAME_GEN`: `gs_task_util`, `gs_render_util`, `gs_mem`, `gs_dvd`,
`gs_flag`, `gs_battle_setup`, `battle/battle_waza`, `gs_colsys`,
`gs_field_resource`, `gs_floor_data`. **7 compiled clean with zero fixes**; the
other 3 needed only:
- `gs_mem` — PREAMBLE forward-decl (`GSmemSplitBlock` used before its in-TU macro).
- `battle_waza` — PREAMBLE forward-decl (`fn_80129280`, the same void*-KR form as
  `battle_main.c`).
- `gs_dvd` — PREAMBLE (`DVDInit`/`memset`) + 2 TEXT_FIXUPS turning the bare-`return`
  `GSDVD_CloseHandle`/`GSDVD_Open` *definitions* from `s32` to `void`
  (-Wreturn-mismatch; the proto rewriters can't touch definition headers).

Build: 54 objects, 0 failed; link converges (undefs 259→408, auto-stubbed).
`--menu`/`--sched-test`/`--engine-boot` (incl. `PCPORT_REAL_TITLE_INIT`) all
unaffected. `src/game` untouched — only the generated copies + `tools/pcport_*`.
This is the foundational lane: every host-linked real TU shrinks the auto-stub
closure and brings more real engine code onto the platform shim.

## 6f. Campaign Phase 1 — host-link all 0-asm game TUs (2026-06-02)

"Attack all remaining asm" (max-effort). Enumerated `src/game`: 102 TUs, 13 already
linked, 89 unlinked = **47 0-asm + 42 asm-bearing**. Phase 1 took the 47 0-asm TUs,
using the **link's compile step as the filter**: add all to `GAME_GEN`, let
`pcport_link` report the compile failures, then fan out a workflow only on those.

- 32/47 compiled **clean with zero fixes**.
- A 15-agent workflow analyzed the 15 failures. **13 merged** (small fixes: a few
  `FUNC_PROTO_RETYPE`/`FUNC_PROTO_KR`/`FUNC_STUB_DROP`/`TEXT_FIXUPS`/PREAMBLE each —
  mostly return-type-unify on pseudo-register protos + forward-decls).
- **2 deferred**: `colosseum_script` (25.7k-line pseudo-register script interpreter —
  needed 165 retypes + 79 *ordered* text-fixups and the agent's own verdict was the
  paths are "non-functional"; pure risk/bloat, off the title/boot path) and `trainer`
  (workflow couldn't resolve). Both stay auto-stubbed = baseline, no loss.

Result: **45/47 0-asm game TUs host-linked.** Build 99 objects, 0 failed; link
converges (962 stubs). `--menu`/`--sched-test`/`--engine-boot` all unaffected.
Running total of host-linked game TUs: 13 → **58**. `src/game` untouched (generated
copies only). Phase 2 (the 42 asm-bearing TUs, `pcport_gen` flips their #if1-asm to
#else C) is next, in chunks with per-chunk regression + bisection.

**Method note for the campaign:** the workflow `args` payload arrives as a JSON
*string*, not an array — the script must `JSON.parse` it (a `(args && args.length)`
guard silently treats the string as the candidate list).

## 6g. Campaign Phase 2 — the asm-bearing tail is functional-decomp, not link-flipping

Phase 2 took the 42 asm-bearing TUs (`pcport_gen` flips their `#if1`-asm to the
`#else` C). Excluded 4 host-shim colliders up front (`main`, `gs_thread`, `gs_task`,
`gs_texture`). Chunk 2a = the 18 low-asm TUs (1–17 asm). The data is decisive:

- **2/18 compiled free** (`gs_particle`, `movie`).
- A 16-agent workflow on the 16 failures yielded only **5 cleanly mergeable**
  (`poke_detail`, `menu_dialog`, `gs_event_exec`, `fsys_load`, `input` — mostly
  forward-decls + neutralizing a few RAW `asm{}` blocks, which aren't `#if1/#else`
  so the flipper can't reach them, to empty stubs).
- **3 pathological** (`colosseum_event` 124 fixes / `pokemon` 111 / `colosseum_battle`
  73 — 10k-line pseudo-register decomps needing dozens of fragile ordered text-fixups
  for *non-functional* code) and **8 unresolved** by the workflow.

So chunk 2a netted **7 host-linked TUs** (→ 65 total); the other 11 stay auto-stubbed
(= baseline). Contrast Phase 1's 0-asm TUs (32/47 free). **Conclusion: the remaining
asm-bearing TUs are real per-function functional decomp** (PPC→C of the still-asm
leaves), not something link-flipping or conflict-fixing can deliver — flipping mostly
yields stubs, and the heavy TUs are fragile non-functional pseudo-register code. The
foundational host-link campaign is therefore **complete at ~65 game TUs**; further
gains require the months-scale Track-D decomp the roadmap always described, prioritized
by the actual title/field/battle call graphs rather than swept wholesale.

Build 106 objects, 0 failed; no regression. `src/game` untouched (generated copies +
`tools/pcport_*` only).

## 6h. P-C step 1 — the first overworld map renders (2026-06-02)

Pivoted from breadth (host-linking) to **depth toward playable**: rendering a real
field/overworld map. The `--field` flag loads a map .fsys and draws it statically
through the existing `scene_data -> RenderJointTree -> fn_800DAD10` path (the same
bridge as the title). **`D1_garage_1F` (Wes's hideout, the game's start) renders** —
the room interior (walls, floor, furniture) verified via `PCPORT_DUMP`.

Two real findings made it work:
1. **Field maps have duplicate-named members.** A map .fsys (e.g. `D1_garage_1F`)
   has several members named after the file; only the largest is the renderable HSD
   scene archive (others are small object sets like `ippan_m_b1`). New
   `PCPort_LoadFsysSceneMember` (real_content_host.c) scans all members, decompresses
   each, and returns the **largest member exposing a `scene_data` public symbol** — so
   the field loader needs no member-index knowledge. The scene_data struct layout is
   byte-identical to the title's (scene_data->branch; branch+0=jointList, +8=camera;
   jointList+0=rootJoint), confirmed by probing both.
2. **The field joint graph contains CYCLES.** It is small (~40 joints, depth 5) but has
   shared sub-trees / back-references that overflowed `RenderJointTree`'s naive
   recursion (STACK_OVERFLOW). Fixed: the sibling (`next`) chain now **iterates**
   instead of recursing, and a **visited-set cycle guard** (reset on the outermost
   call via a depth counter) skips already-drawn joints. Behaviorally identical for the
   title (still joints=31/dobjs=43/drawn=35) — it's a pure robustness fix.

`PCPort_EngineFieldSetup` reuses the title render statics; the map's embedded camera
didn't translate so a default look-at is used (`PCPORT_CAM_EYE`/`_INT` tune it;
`PCPORT_FIELD_ARCHIVE` picks the map). Build 106 obj/0 fail; `--menu`/`--sched-test`/
`--engine-boot` unaffected. **Next P-C steps:** texture/material fidelity on the map,
then the WZX collision mesh + player update for actual walking (`gs_field_colquery`/
`gs_colsys` cluster) — the first truly *playable* milestone.

## 6i. P-C step 2 — interactive field exploration (free-fly camera) (2026-06-03)

`--field` is now **interactive**: a free-fly camera you steer through the loaded map.
Per frame it `PADRead`s the pad shim (GLFW keyboard) and updates a yaw/pitch + eye
camera: W/S forward, A/D strafe (main stick = WASD), arrow keys turn/look (d-pad),
C-stick (IJKL) looks, Z(=A) rise / X(=B) sink; the look-at view matrix is rebuilt and
fed to the same `RenderJointTree` render. `PCPORT_FIELD_AUTOPAN=1` drifts the camera
forward + pans for headless verification (verified: eye 0,90,330 -> 44,72,190, yaw
0->0.60 over 50 frames, the dump showing a new in-room viewpoint). `PCPORT_CAM_EYE`
sets the start position; `PCPORT_FIELD_ARCHIVE` picks the map.

**Green background fixed.** `GSgfx_BeginFrame` (the real engine setup) paints a green
EFB clear-quad (RGB 48,213,94) that the title covers with its sky/ground but a sparse
field map doesn't — so it showed through. Since `src/game` is read-only, field mode
(`g_engFieldMode`, set by `PCPort_EngineFieldSetup`) now **re-clears to a chosen
background AFTER `GSgfx_BeginFrame`** (default near-black; `PCPORT_FIELD_BG="r,g,b"`
overrides). The map now renders on a clean dark background (corner px 10,13,20 vs the
old 48,213,94); the title path (`g_engFieldMode==0`) is untouched.

This makes the first overworld map **explorable** — the bridge from "renders" to
"walkable". Only the field path changed (`--menu`/`--sched-test`/`--engine-boot`
untouched). Remaining field cosmetic: textures/materials still read washed-out (the
material/TEV path doesn't fully apply on the map — same class as the title's tan-on-tan).
**Next:** a player avatar + the WZX collision mesh so
movement is floor-clamped and wall-blocked (real walking), via the
`gs_field_colquery`/`gs_colsys` cluster.

## 6j. Title camera pan-out (match GameCube intro) (2026-06-03)

The GameCube title renders the desert ruins in 3D and pulls the camera OUT to the wide
title view; the port jumped straight to the static end pose. Added a **one-shot intro
pan** in `RunMenuScene`: each frame (over `panSecs`≈4.5, ease-out cubic) it lerps the
camera eye/interest from a close/low start `(0,20,140)`→`(0,52,-70)` to the authentic
wide title end `(0,38.9,409.8)`→`(0,39.65,1.56)` (`cam_logo_demo_stop`), then holds.
`PCPORT_NO_PAN` disables; `PCPORT_PAN_SECS` sets duration; a manual `PCPORT_CAM_EYE`
override suspends it. The clock is the existing title anim clock (`PCPORT_ANIM_TIME`
pins it headless).

**Honest finding from the camera data:** the real `cam_logo_demo_start` and
`cam_logo_demo_stop` differ by only a ~10-unit eye dolly with the same look-target — the
title scene's own GameCube camera move is subtle. The dramatic "fly through Orre" is the
**opening-demo movie** (`openingdemo.thp`, which the port already plays before the title).
So this pan is a slightly-more-cinematic pull-out anchored on the authentic end pose, not
a 1:1 of the (nearly-static) real title camera.

**Logo bounce-in (DONE).** Per a GameCube reference sequence the user captured (close-up
up at a ruins column → pull out to the wide ruins → logo bounces in → cast/PRESS START
follow), the foreground UI is now hidden during the pan and the logo **bounces in** after
the camera settles: a back-out (overshoot) scale about the logo centre over ~0.55s once
`aT >= panSecs`; cast + PRESS START + copyright appear ~0.45s later (`uiAfter`). During the
pan only the 3D ruins show. `PCPORT_NO_PAN` makes everything immediate.

**Boot-flow fixes (DONE, per user corrections).** (1) The opening-demo movie is no longer
played during boot — it's now a **title idle attract loop**: after `demoIdleSecs` (≈22s,
`PCPORT_ATTRACT_SECS` / `PCPORT_NO_ATTRACT`) of no input on the title, `RunMenuScene` plays
`openingdemo.thp` via `BootPlayTHP`, then returns to the title (input resets the timer).
(2) **Boot logos fade in AND out** — `BootShowLogo` now draws via `DrawTexturedScreenRectA`
with a time-based alpha (fade-in over 0.45s, hold, fade-out over 0.45s, over black). Boot
is now Nintendo→TPC→Genius Sonority only.

Known remaining gap: the **ruins are low-contrast** (tan-on-tan) — the GameCube columns are
crisp sandstone; a per-mesh material/texture-assignment issue (the title's logo/cast/sky DO
texture correctly, so the TEV path works — it's a map/pillar material-resolution gap),
separate from the camera. Only the title/boot path changed; `--field`/`--sched-test`/
`--engine-boot` untouched. (The boot/attract/fade are timing behaviors verified by a clean
build + title-render smoke + no-crash; full visual check is interactive.)

## 6. Constraints honored

Edited only `src/pcport/**` + `tools/pcport_*` + this doc. No `*_fn_*.inc`,
symbols, splits, `objdiff.json`, or CW configs touched; no `#if 0`→`#if 1`
asm-wrapper flips; no byte-match decomp. `src/game/*` and `src/dolphin/*` were
read-only references. The `--menu` front-end is verified still working.

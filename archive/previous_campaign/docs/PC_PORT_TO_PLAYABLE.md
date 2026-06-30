# Pokémon Colosseum PC Port — Roadmap to a *Playable* Game

> **Status:** strategic audit, 2026-06-02. Companion to `pc_port_roadmap.md` (2026-05-31,
> still valid on the pivot + Track-D rules) and `PC_PORT_HANDOFF.md` (the front-end build
> log). This doc updates the *current state* (the front-end is now done) and lays out the
> concrete path from "front-end demo" → "you can actually play it." Read-only planning;
> no code touched.

---

## 0. The one-line state

The port has a **complete, faithful front-end** — boot videos → animated title → memory-card
prompt → main menu → dialogs — all from real assets via the game's own draw path + host shims.
It is **not yet a game**: nothing past the menu runs real *gameplay* (no field/overworld you can
walk, no battle). Closing that gap is what "playable" means, and it hinges on one strategic
decision (below).

---

## 1. What actually works now (June 2, all on master)

- **Window + GPU path:** GLFW/GL window; real disc assets (FSYS+LZSS) → big-endian HSD scene
  parse → the game's own `fn_801AA568 → fn_800DAD10` draw bridge → pixels. Texture decode
  (CMPR/RGBA8/RGB5A3/I8/CI+TLUT), face-normal directional lighting.
- **Front-end flow:** `RunMenuScene` is a host-driven state machine: boot THP videos
  (Nintendo→Pokémon→GS→opening) → 3D desert-ruins title (full scene after the DObjDesc-next
  fix) → posed 2D-cutout cast (`t_vs_*`) that **cycles** sets on idle → cloud-drift + sand-wind
  ambient anim (commit 441ee9a8) → PRESS START → save-presence prompt → main menu (Story/Battle/
  Options composited from real sprites + a bitmap font) → Yes/No + info dialogs.
- **Input:** GLFW keyboard via `pad_shim.c` (`PADRead`), host edge-detector.
- **THP video:** full decoder (the FF00-restuffing trick), plays muted.

## 2. What "playable" requires (the gap)

To go from front-end → playable, the port must run **real game-mode logic**, not just render
static scenes + 2D menus. Concretely, the smallest meaningful "playable" milestone is **Story
Mode field/overworld**: load a map, see your character, **walk around with collision**, trigger
events/NPC dialogue. The larger milestone is **battle** (turn logic, Pokémon models, moves).

Both need the game's *own* per-frame logic loop to actually execute — which is the deferred
strategic decision.

---

## 3. THE strategic fork (decide before P-A)

The front-end was built **host-driven**: our C (`RunMenuScene`) calls individual game render
fns and we drive the state machine ourselves. That works for menus/static scenes. It does **not**
scale to gameplay, because field/battle logic is thousands of interdependent game functions
driven by the engine's own scheduler. Two ways forward:

- **(A) Host-driven reimplementation** — keep writing our own loop and call game logic piecemeal.
  *Verdict: not viable for gameplay* — you'd be reimplementing the game, not porting it.
- **(B) Authentic engine-hosting** — run the game's real boot/scheduler so its own field/battle
  code executes. This is the deferred LARGE item from `PC_PORT_HANDOFF.md §3(1)`. It needs a host
  **cooperative-fibre runtime** for:
  - `fn_800A263C` (fibre create), `fn_800F0308` (per-frame vsync yield the whole engine blocks on),
  - host stand-ins for the asm-only SI poll `fn_800AB150` / `fn_800D0F44`,
  - the engine TUs linked: `main.c`, `gs_thread.c`, `gs_task.c`, `gs_title.c`, `input.c`, …
  *Verdict: this is the real path to playable.* Everything below assumes (B).

> **Recommendation:** commit to (B). The front-end proved the render + shim + asset stack is
> sound; the remaining work is hosting the engine loop + converting the still-asm on the
> field/battle call graphs to functional C (Track D).

---

## 4. Critical path to playable (phases, deps, effort)

| Phase | Deliverable | Depends on | Effort | Notes / risk |
|---|---|---|---|---|
| **P-A** | **Host fibre/scheduler runtime** | — | **LARGE** | The keystone. Implement `fn_800A263C` fibre-create + `fn_800F0308` vsync-yield on a host cooperative scheduler (ucontext/Win-fibers/longjmp); stand in the SI-poll fns. Until this runs, no game-mode logic executes. |
| **P-B** | **Boot the engine to its own title/menu** | P-A | MEDIUM | Link `main.c`/`gs_thread.c`/`gs_task.c`/`gs_title.c`/`input.c`; let the engine drive the title itself (replacing our host-driven `RunMenuScene` for the in-engine path). Proves the loop runs real game state. The current front-end stays as the fallback/boot shell. |
| **P-C** | **Field/overworld: load + render + WALK** | P-B | LARGE | Load a map fsys (`D1_*`/`D2_*`), run the field scene + player update; input→movement→collision via the game's field code. First "playable" milestone. Surfaces the `gs_worldmap.c` (~39% asm) + field-TU Track-D worklist. |
| **P-D** | **Real save / game-state** | P-B | MEDIUM | Replace the host save-presence stub with real load/new-game state so Continue/New Game enter the world. GCI/SHA-1 only if Dolphin cross-save is wanted. |
| **P-E** | **Battle** | P-C | LARGE | Turn logic + Pokémon model render + moves/effects. Biggest gameplay system; its own large Track-D cluster (battle_*). |
| **P-F** | **Audio** | parallel | MEDIUM | BGM + SFX + the THP ADPCM (currently muted). `audio_shim.c` is stubbed; independent track, can run anytime. |
| **P-G** | **TEV→GLSL shader backend** | parallel | MEDIUM-LARGE | Today rendering is fixed-function-ish + face-normal lighting. Full material/fog/alpha-test fidelity needs the generated-shader backend (`gx_tev.c` still largely TODO). Needed for correct field/battle visuals, not strictly for "it runs." |

Parallelizable: **P-F (audio)** and **P-G (shaders)** don't block P-A/P-B and can proceed
independently (good agent lanes). P-A is the serial keystone for everything gameplay.

---

## 5. Track D (functional decomp) the above gates

Per `pc_port_roadmap.md` rules: convert **only** the still-asm functions on the port's *gameplay*
call graphs to **functional** C (NOT byte-match — the WALLS.md walls don't apply on x86). Build a
call-graph-reachability tracker from the new entry points (the engine boot chain, the field/battle
scenes) to get the exact worklist. Known heavy clusters:
- **Engine/loop:** the fibre/scheduler + SI-poll fns (P-A) — must be functional C or host shims.
- **Field:** `gs_worldmap.c` (~39% asm) + the field/event TUs.
- **HSD transform/draw leaves:** `hsd_cobj` (74% C) / `hsd_jobj` (79%) / `hsd_mobj` (77%) — the
  matrix + primitive-draw hot paths the scene render still drops into asm for.
- **Battle:** the `battle_*` cluster (P-E).
(`gs_gfx`/`gs_model`/`hsd_pobj`/`hsd_tobj`/`hsd_render`/`hsd_tev` are already 100% C.)

The decomp.me queue pipeline (separate track) can feed Track D — but prioritize *port-reachable*
functions over arbitrary near-misses.

---

## 6. Recommended next concrete step

**P-A spike: a minimal host fibre runtime + the vsync-yield shim**, then attempt to boot
`main.c`/`gs_thread.c` far enough to print one engine frame. This is the highest-leverage,
highest-uncertainty item — de-risk it first with a small spike before committing to full P-B.
If the fibre runtime proves out, the rest is "link more TUs + convert the still-asm they hit."

If P-A looks too big to start cold, the safe warm-ups that build toward it without the fibre risk:
- **P-F audio** (independent, visible win: sound on the boot videos + title BGM).
- **A field-scene *render-only* spike** (load a `D1_`/`D2_` map and render it statically via the
  existing `RenderJointTree`, no movement) — proves map assets load + reuses the title pipeline,
  and scopes the field Track-D worklist, without needing the engine loop yet.

---

## 7. Honest assessment

The front-end is ~done and faithful — a real accomplishment. But "playable" is a **large** step
up: it requires hosting the engine's cooperative scheduler (P-A) and converting the field/battle
still-asm to functional C. This is months-scale, not a session. The good news: the architecture
is proven, the pivot (functional C, not byte-match) makes the decomp tractable, and the work
parallelizes cleanly (audio + shaders + per-cluster decomp as independent lanes once P-A lands).

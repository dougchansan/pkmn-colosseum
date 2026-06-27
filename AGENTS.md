# AGENTS.md — Pokémon Colosseum Decompilation Guardrails

> **New here? Read [`AGENT_ONBOARDING.md`](AGENT_ONBOARDING.md) first** — the quickstart loop, the one authoritative measure command, the lever cheat-sheet, and the hard rules to start decompiling immediately.

This repository is a matching decompilation project for Pokémon Colosseum. The highest priority is preserving or improving byte-for-byte matching of the original `main.dol`. Do not make cosmetic, broad, or speculative changes that risk breaking matching.

## Project context

- Target game: Pokémon Colosseum, GameCube, `GC6E01` / `GPXE01` context where applicable.
- Primary source tree: `src/`.
- Headers: `include/`.
- Config, symbols, splits, and build metadata: `config/GC6E01/`.
- Local generated assembly may exist under `asm/`, but generated or ROM-derived files should not be treated as normal editable source.
- The project uses a Metrowerks CodeWarrior-based matching workflow. Small C changes can change assembly output.
- Matching work should be validated with the repo tools, not just by whether the C compiles.

## Hard rules

- Do not edit `*_fn_*.inc` files.
- Do not add, stage, or commit any `.inc` truth/asm snippet files. They are ROM-derived scaffolding, not decompiled C, and public PRs must not contain them. If a local tool needs generated `.inc` files, keep them ignored/local only.
- Do not flip asm-wrapper scaffolding `#if 0` -> `#if 1` to force a fake match. An asm-wrapper re-embeds the original `main.dol` machine code: it byte-matches by definition but is **0% decompiled** and is useless for the PC port (x86 cannot run PPC asm). Flipping a function that has real C (even a near-miss) back to an asm wrapper is a REGRESSION of honest decompilation, even though the objdiff/decomp.dev number ticks up. The ONLY legitimate asm-wrapper is for a **confirmed, logged wall** (real C written and proven unable to byte-match due to a toolchain artifact) — and it must be recorded in `WALLS.md` / `tools/decomp_work/equivalent.txt` with the real C parked under `#else`. See "Current campaign" below for the validated policy.
- Do not accept asm wrappers, inline `asm`/`__asm`, or `#include .inc/.s` bodies as decompilation wins. They are fraud for the PC-port goal.
- Do not count register-dump or pointer-arithmetic C as done. A finished decompilation must be readable, typed C. Use typed structs, named fields, arrays, enums, and helper accessors when evidence supports them. If raw pointer arithmetic appears in a scratch candidate while a struct is unknown, keep it narrowly scoped, document the evidence gap, and track it as cleanup debt rather than calling it a decompilation win.
- Do not commit or generate copyrighted game assets, extracted ROM contents, compiler binaries, or target objects into the repo.
- Do not rewrite control flow just to make code look cleaner.
- Do not rename jump tables, labels, linker-required symbols, or config symbols unless specifically requested and all references are updated.
- Do not perform broad repository-wide renames without first producing a reviewable rename audit.
- Do not invent confident names for unknown functions, globals, structs, or fields.
- Do not change public function signatures, struct layout, field signedness, array dimensions, packing, or typedef widths unless there is strong evidence and the change is validated.
- Do not remove address comments, original symbol references, or useful decomp notes unless they are superseded by better documentation.

## Build and validation expectations

When changing C or headers, prefer the smallest validation that proves the change is safe:

1. For one source file:
   - `python tools/compile_check.py src/path/to/file.c`
2. For matching/diff work:
   - `python tools/decompctx.py src/path/to/file.c`
   - use decomp.me or local objdiff as appropriate
   - `./tools/objdiff-cli diff -1 <target.o> -2 <yours.o>` when target objects are available
3. For progress checks:
   - `python tools/progress.py`
4. For full build verification:
   - `ninja`
   - verify SHA-1 when the local environment has the required original files

If the environment cannot run a command, say so explicitly and provide the exact command the user should run locally.

## Naming philosophy

Rename based on evidence, not vibes.

A good name must be supported by at least one of:

- multiple callsites showing consistent behavior
- known library pattern, such as `memset`, `memcpy`, `qsort`, allocator/free, UI show/hide, etc.
- resource IDs or string references
- data-table usage
- struct field offsets used consistently across several functions
- known hardware/API convention
- matching decomp project convention
- Ghidra/decompiler evidence confirmed by assembly

Use conservative temporary names when confidence is not high.

Examples:

- `CardE_CompareEntries` is acceptable if the function is used as a comparator for an entry pointer array.
- `qsort_like` is acceptable if callsites pass array pointer, count, element size, and comparator.
- `UI_SetVisible` may be acceptable as medium-confidence if callsites repeatedly pass UI/pane pointers and `0`/`1`.
- `CardE_DoThing` is not useful.
- `LoadPokemonCardTextureAndAnimateSelection` is too specific unless proven.

## Confidence levels

Every nontrivial rename proposal must include a confidence rating.

### High confidence

Use when the behavior is clear from direct evidence.

Examples:

- function is passed as a comparator to a qsort-like helper
- function directly wraps a known libc-like pattern
- field is clearly a count because it controls loop bounds and allocation size
- field is clearly a pointer array because it is indexed by `index * 4`

### Medium confidence

Use when evidence is strong but not final.

Examples:

- function appears to show/hide UI panes but exact API name is unknown
- field appears to be selected index but could be cursor index or list index
- resource lookup function returns UI objects, but exact resource type is unknown

### Low confidence

Do not apply the rename by default. Add a comment or naming proposal only.

Examples:

- guessed from one callsite
- guessed from visual behavior without data evidence
- guessed from nearby function names
- guessed from a resource ID alone

## Rename workflow

Never rename a symbol by editing only one occurrence.

For each rename:

1. Search the entire repository for the old symbol.
2. Update the definition.
3. Update forward declarations.
4. Update extern declarations.
5. Update headers.
6. Update callsites.
7. Update function pointer references.
8. Update relevant comments or mapping notes.
9. Run a final search for the old name.
10. Report any remaining references.

If remaining references are intentional, explain why.

For a requested batch rename, first produce a rename audit. Do not edit files until the audit is reviewed or the user explicitly asks to apply high-confidence renames.

## Rename audit format

Use this table format:

| Old name | Proposed name | Kind | Confidence | Evidence | Safe to apply now? |
|---|---|---|---|---|---|
| `fn_8007FD64` | `CardE_CompareEntries` | function | High | Passed as comparator to qsort-like function; compares pointed-to entries by stable offsets | Yes |

Kinds include:

- function
- global
- rodata label
- jump table
- struct
- field
- local variable
- typedef
- enum
- macro

## Function naming rules

Prefer names shaped like:

- `System_ActionObject`
- `System_GetThing`
- `System_SetThing`
- `System_UpdateThing`
- `System_DrawThing`
- `System_InitThing`
- `System_FreeThing`
- `System_ParseThing`
- `System_CompareThings`

Examples:

- `CardE_CompareEntries`
- `CardE_InitMenuContext`
- `CardE_ReloadEntries`
- `CardE_ParseBitstream`
- `CardE_ApplyDecodedField`
- `Menu_SetPaneVisible`
- `Resource_GetById`

Avoid:

- names that describe implementation details only, such as `LoopThroughArray`
- names that include unproven UI meaning, such as `ShowSelectedPokemonShadowCard`
- names that erase useful uncertainty, such as replacing `field_C0` with `selectedPokemonMoveSlot` without proof

## Address-based names

Preserve original address-based traceability.

When renaming an address-named function, keep the old address in a comment if the project convention does not already preserve it.

Preferred:

```c
/* 0x8007FD64 */
s32 CardE_CompareEntries(const void* a, const void* b) {
    ...
}
```

Acceptable temporary approach:

```c
// TODO: likely CardE_CompareEntries
void fn_8007FD64(void) {
    ...
}
```

Use the temporary approach when a project-wide rename has not yet been performed.

## Struct recovery rules

Partial structs are encouraged, but only for supported offsets.

When creating or extending a struct:

- preserve exact size and alignment
- preserve known offsets with padding
- use `unk_XX` or `field_XX` for uncertain fields
- do not reorder fields
- do not infer signedness unless the assembly supports it
- do not convert arrays/pointers unless indexed usage proves it
- include offset comments when helpful

Preferred style:

```c
typedef struct CardEEntry {
    u8 unk_00[0x1A];
    u8 entryId;      /* 0x1A */
    u8 unk_1B;       /* 0x1B */
    s8 sortGroup;    /* 0x1C */
    s8 subIndex;     /* 0x1D */
} CardEEntry;
```

For uncertain context fields:

```c
typedef struct CardEMenuContext {
    u8 unk_000[0xA0];
    s32 prevIndex;          /* 0xA0 */
    s32 selectedIndex;      /* 0xA4 */
    void* selectedEntry;    /* 0xA8, if proven */
    s32 entryCount;         /* 0xAC */
    CardEEntry** entries;   /* 0xB0 */
    u8 prevSubIndex;        /* 0xB4 */
    u8 subIndex;            /* 0xB5 */
    u8 transitionActive;    /* 0xB6 */
    u8 unk_0B7;             /* 0xB7 */
    s32 transitionFrame;    /* 0xB8 */
} CardEMenuContext;
```

If a field is only suspected, keep it as `field_XX` and add a comment with the suspicion.

## Local variable naming rules

Local variables may be renamed more freely than global symbols, but still must be evidence-based.

Good local names:

- `ctx`
- `entry`
- `entryCount`
- `entryIndex`
- `pane`
- `resourceId`
- `frame`
- `state`
- `oldIndex`
- `newIndex`

Avoid renaming locals when doing so forces broad expression rewrites that may affect matching.

If a local still maps clearly to a PowerPC register in a nonmatching function, it is acceptable to leave names like `r30` until the function is better understood.

## Jump table and switch rules

Jump tables are matching-sensitive.

- Do not rename jump table labels unless required by project convention.
- Do not hand-edit jump table order.
- Do not convert a jump table to if/else unless the original compiler output used branches.
- When a jump table is present, identify the switched variable and document the case mapping first.
- If a switch is reconstructed, ensure case order and default behavior match the target assembly.

For a jump table audit, use this format:

| Case | Target label/address | Probable meaning | Confidence | Evidence |
|---:|---|---|---|---|
| 0 | `0x8007FCC0` | unknown/default-like state | Low | target from jump table only |

## Matching-sensitive C rules

Do not casually change:

- signedness
- temporary variable lifetimes
- expression order
- pre/post increment style
- loop shape
- `if` nesting
- `switch` layout
- casts
- volatile access
- float constants
- double vs float operations
- struct field types
- array types
- inline functions
- function prototypes
- `static` / `extern`
- file order or weak symbol order

Even if the code looks ugly, preserve it unless the change is part of a deliberate matching attempt.

## Comments and documentation

Comments should record evidence, not speculation.

Good:

```c
/* entries is indexed as selectedIndex * 4 and stores pointers returned by CardE_GetEntryByIndex. */
```

Bad:

```c
/* This is definitely the Pokemon card texture list. */
```

Use `TODO`, `LIKELY`, or `MAYBE` for uncertain notes.

## Card-E naming notes

The Card-E/menu code currently has several evidence-backed patterns that can guide conservative naming:

- Some functions around `0x8007C300` to `0x80082650` manage Card-E menu/matrix state.
- A context-like object uses offsets such as `0xA0`, `0xA4`, `0xAC`, `0xB0`, `0xB4`, `0xB5`, `0xB6`, and `0xB8` repeatedly.
- Offset `0xAC` behaves like an entry count.
- Offset `0xB0` behaves like an array of entry pointers.
- Offset `0xA4` behaves like a selected/current entry index.
- Offset `0xA0` behaves like a previous entry index.
- Offsets `0xB4` and `0xB5` behave like previous/current sub-selection or page state.
- Offset `0xB6` behaves like a transition/animation active flag.
- Offset `0xB8` behaves like a transition frame counter.
- Card-E entries appear to use byte fields around `0x1A`, `0x1C`, and `0x1D` for ID/sort/sub-index behavior.
- `fn_8007FD64` appears to be a comparator for Card-E entry pointers.
- `fn_80083BF8` appears to return a Card-E entry count.
- `fn_80083AF4` appears to return a Card-E entry by index.
- `fn_800CA620` appears qsort-like.
- `fn_80109220` appears to set UI/pane visibility or enabled state, but exact naming is medium-confidence.

Treat these as starting hypotheses. Confirm before applying broad renames.

## Suggested first-pass Card-E rename audit targets

Start with these only, and only apply after audit:

| Old name | Suggested temporary name | Confidence |
|---|---|---|
| `fn_8007FD64` | `CardE_CompareEntries` | High |
| `fn_8007FDBC` | `CardE_InitMenuContext` | Medium-high |
| `fn_8007C7EC` | `CardE_ReloadEntries` | Medium-high |
| `fn_80083BF8` | `CardE_GetEntryCount` | Medium |
| `fn_80083AF4` | `CardE_GetEntryByIndex` | Medium |
| `fn_800CA620` | `qsort_like` | High |
| `fn_80109220` | `UI_SetVisible` or `Pane_SetVisible` | Medium |

Do not apply low-confidence names globally.

## Codex task patterns

### Naming audit only

Use this behavior when asked to identify names:

```text
Analyze the requested files and produce a naming audit only. Do not edit files. For each symbol, give old name, proposed name, kind, confidence, evidence, and whether it is safe to apply now.
```

### Safe rename application

Use this behavior when asked to apply names:

```text
Apply only high-confidence renames. Update definitions, declarations, externs, headers, callsites, and function pointer uses. Do not rename jump tables or linker-required labels. Do not change function bodies except for symbol names and type names required by the rename. Run a final search for old names and summarize remaining references.
```

### One-function cleanup

Use this behavior for a single function:

```text
Focus only on the requested function. Identify arguments, important locals, struct offsets, callsites, and control-flow shape. Rename locals conservatively. Do not refactor control flow unless required for matching and explicitly requested.
```

## Response format for agents

When reporting work, always include:

1. What changed.
2. Why it was safe.
3. What evidence supports each rename.
4. What validation was run.
5. Any commands that could not be run.
6. Any remaining risks or uncertain names.

Keep diffs small. Matching progress is more important than cosmetic readability.

## Current campaign & targets (2026-06-13)

Goal: a fuzzy-match attempt on EVERY remaining function, converting asm-wrappers and stubs to real, byte-matching C — file by file, agent by agent, model-tiered. Token budget is large (multi-day); spend it evenly across attack vectors. The PC port needs real C, so **real C beats asm-wrappers** everywhere it can.

### Coordination (do not conflict)
- `src/game/gs_field_world.c` is owned by a live **codex** session — do NOT touch it.
- Never edit, add, stage, or commit `*_fn_*.inc` / `.inc` truth files. They are not C and they must remain ignored/local if generated.
- Do not edit `config/GC6E01/symbols.build.txt` blindly.
- After any sub-agent claims a match, RE-MEASURE in the parent before trusting it (sub-agents have hallucinated 100%).

### The metric (three honest axes — know which you're moving)
1. **scan C-CONVERTED ~83%** (`progress2.py`, 6897 real-C / 8307 src fns) = "how much C is written."
2. **objdiff ROM fn-match 58.2%** (5305/9120) = the **decomp.dev / README "Function match"** number. Counts asm-wrappers as matched, so it over-states true decomp.
3. **byte-exact C 41.9%** (3819/9120) = real C that byte-matches (the strict, honest axis).
Renames are byte-neutral (don't move axes 2/3). The right way to grow the number is convert asm/stub -> byte-exact C, plus byte-neutral `symbols.build.txt` reloc-name renames.

### asm-wrapper policy (validated 2026-06-13)
Asm-wrappers ONLY for confirmed walls (logged in `WALLS.md`/`equivalent.txt`). Never wrap a winnable function to inflate the number. If real C can't reach 100%, leave the real C **active** and log it as an Equivalent — do not re-wrap as asm unless the byte-perfect DOL build specifically requires it AND it's a logged wall.

### Readability policy (current)
The destination is readable, typed C that still byte-matches. A function that is just register names, raw offsets, or broad pointer arithmetic is only an intermediate recovery artifact, not the finish line. Prefer evidence-backed struct recovery and conservative names over permanent `*(T*)((u8*)p + off)` code. If byte-matching currently requires raw access, isolate it and leave a follow-up target for struct/field recovery.

### Verification protocol
- Per-fn match: `python tools/match_scan_file.py <src/file.c> <fn_...>` (compiles file + objdiff per symbol).
- Whole-tree honest axes: `python tools/decomp_work/progress2.py [--measure]`.
- Parent gate before commit: `python tools/decomp_work/overnight/verify_match.py <stem> <fn>`; gate commits with `verify_gate.py --range HEAD~1..HEAD` (diff-aware, not `--files`).
- Avoid `compile_check.py -s` for per-fn % (under-reports, corrupts base/.o).

### Model tiering (delegate by tool-use / difficulty)
- **opus** — the giant individual asm fns + heavily-walled files: `fn_80059BDC` (3392 instr, `ui/ui_core.c`), `fn_801CDB04`/`fn_801CC380` (1720/1505, `battle/battle_scene.c`), `fn_801920E4` (1637, `hsd/hsd_aobj.c`), `fn_800E732C` (1350, `gs_material.c`), `scene_init.c` cluster (`fn_80051E38`/`80046168`/`80042658`, 900-1268), `people_data.c::fn_80141308` (1263). Highest-value FILES: `gs_field_world.c` (codex), `ui/ui_core.c` (70 asm/30 realc).
- **sonnet** — medium 81-500 instr asm + stub-conversion files: `battle/battle_waza.c` (104 stubs), `gs_model.c` (70), `gs_field_colquery.c` (61), `gs_colsys.c` (31), `effect/effect_visual.c` (40 small fresh asm, no walls), `people/people_field.c`, `gs_npc_interact.c`, `gs_worldmap.c`.
- **haiku / deepseek-v4-flash (no-think)** — leaf/mechanical: `dolphin/gx/GXInit.c` (~120 small SDK register-write wrappers, GX-SDK-documented), 94 leaf asm fns <=30 instr, `effect_visual.c` tail. deepseek lane: `tools/decomp_work/overnight/run_deepseek_pane.sh` (broad-drain queue).

### Walls to SKIP (don't grind; in WALLS.md/equivalent.txt)
scene_init `fn_80040308`/`8004B7EC`/`8004C4A4`; gs_material `fn_800E3604`/`800E8EFC`/`800E4170`/`800E5550` family; gs_field_world `fn_8011A280` band + `fn_80129280`; gs_thread `fn_800FE38C`/`800F7434`; pokemon `fn_801F54A4`; input.c pad-lookup `fn_800F7DE4` family; HSD `W-SDA-WRAPPER` set. `@nnn` int->float conversion-literal relocs cap at ~99.85 — log and move on.

### Fable's reusable levers (apply across files)
- **symbols.build.txt reloc-name rename** (byte-neutral, scriptable, ~25 fns/day): our C calling `fn_XXXX` mismatches the reloc NAME only; rename the C identifier to the `symbols.build.txt` name -> instant 100% when it's the sole residual. Re-run across HSD cluster + colosseum_script. Delete stale K&R `extern void Name();` that collide.
- **`-O4,s` stmw flag**: CW emits `stmw` at 2-3 saved regs (target idiom) under `-O4,s`, not `-O4,p`. Apply per-TU (`compile_config.json`) or `#pragma optimize_for_size on`; gate on full-TU 0-regression. NOT yet on `colosseum_battle.c` (647 incs).
- **`void fn(void)` pseudo-register -> real signature**: Ghidra-imported fns with `u32 r3; r3=0;` can't keep a param; give the real signature. Bulk win in `menu/*` files.
- **file-scope varargs prototype**: `crset cr1eq` vs `crclr` before a variadic `bl` = a dropped FP arg; add `extern T fn(fixed, ...);` at every decl site + pass the missing float. Ripples to siblings. (gs_render, gs_npc_interact, effect_visual.)
- **typed-field flag ops -> `rlwinm`/`oris`**: rewrite `*(u32*)p |= 0xNNN` as `((T*)p)->flags |= 0xNNNu` (the `u` suffix is load-bearing for high-bit ORs). (gs_material, gs_render, hsd_dobj/tev.)
- Reg-alloc levers (block-scope decl demotion, param-split arg/ctx) are **opt-level-gated** — dissolved by copy-prop at `optimization_level 4`; only bite below opt4.
- **Uncommitted Fable wins to harvest/verify**: `gs_title.c::fn_80022834` (96.83% in tree), `scene_init.c::fn_8003686C` (98.32%), `gs_field_world.c::fn_80125390` (98.78%, codex's file).

## Imported Claude Cowork project instructions

continue the decompilation work

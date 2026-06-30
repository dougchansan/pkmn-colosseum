# Status Audit - 2026-04-02

## Scope and Method

- Repo-local audit only. No network or MCP data was used.
- Commit review was limited to the last 20 commits.
- Conclusions are grounded in the current tree, build/config files, and recent history.
- The worktree is dirty; unrelated user changes and scratch files were not edited.

## Repo Status Summary

### Verified

- The GameCube decomp/matching workflow is the primary supported path:
  - `configure.py` generates `build.ninja`
  - `tools/compile_check.py`, `tools/match_test.py`, `tools/assign_work.py`, and `objdiff.json` are present and wired into the repo
  - `config/GC6E01/compile_config.json` sets `GC/1.3` as the default compiler and applies `GC/1.2.5n` overrides for selected files
- The original assets needed for local work are already present in this checkout:
  - `orig/GC6E01/game.iso`
  - `orig/GC6E01/start.dol`
- The PC-port path exists only as scaffolding:
  - `CMakeLists.txt` is pcport-only
  - `GAME_SOURCES` and `HSD_SOURCES` are still empty
  - `src/pcport/*.c` are TODO-heavy skeletons/stubs

### Source Tree Snapshot

- `src/` contains 215 C files and 7 local headers.
- `include/` contains 74 headers.
- `src/` contains 7,635 `.inc` wrapper files.
- `asm/GC6E01/nonmatching/` currently contains 23 `.s` files.
- 80 C files still contain `asm` function definitions.
- Across those files:
  - 1,242 wrapper blocks are currently switched to C (`#if 0` around the asm path)
  - 1,346 wrapper blocks are still switched to asm (`#if 1` around the asm path)
  - 2,690 `asm` function definitions remain in-tree
- Remaining asm-backed work is concentrated in game code:
  - `game`: 44 files with asm, 2,329 asm definitions
  - `hsd`: 13 files with asm, 218 asm definitions
  - `dolphin`: 15 files with asm, 116 asm definitions

### What Appears Matched or High-Confidence Decompiled

- Fully C game files with no active asm definitions include:
  - `battle_grid.c`, `battle_main.c`, `battle_waza.c`
  - `sound.c`, `sound_bgm.c`, `sound_se.c`
  - `gs_mem.c`, `gs_model.c`, `gs_field_resource.c`, `gs_colsys.c`, `gs_dvd.c`
  - many data/menu/save/gba/script files
- Mixed files that are now mostly or entirely switched to C include:
  - `battle_logic.c`: 130 active C wrappers, 0 active asm wrappers
  - `gs_material.c`: 127 active C, 11 active asm
  - `scene_init.c`: 199 active C, 59 active asm
  - `gs_render.c`: 204 active C, 54 active asm
  - `effect_util.c`: 77 active C, 25 active asm
  - `people_data.c`: 53 active C, 8 active asm
  - `gs_pcbox.c`: 54 active C, 8 active asm
  - `gs_task.c`: 39 active C, 5 active asm
  - `movie.c`: 14 active C, 3 active asm
  - `hsd_dobj.c`: 18 active C, 2 active asm

### Stubbed / Placeholder / Not Yet Decompiled

- The pcport layer is explicit placeholder code today:
  - `gx_shim.c`, `gx_tev.c`, `gx_texture.c`, `audio_shim.c`, `dvd_shim.c`, `pad_shim.c`, `os_shim.c`
  - all still contain scaffold comments such as "skeleton only", TODO sections, or not-implemented returns
- Large mixed TUs still rely heavily on asm-backed wrappers:
  - `gs_field_world.c`: 85 active C, 161 active asm
  - `people_field.c`: 9 active C, 115 active asm
  - `gs_thread.c`: 2 active C, 105 active asm
  - `effect_visual.c`: 0 active C, 89 active asm
  - `ui_core.c`: 14 active C, 87 active asm
  - `hsd_cobj.c`: 0 active C, 62 active asm
  - `gs_npc_interact.c`: 0 active C, 47 active asm
  - `people.c`: 7 active C, 46 active asm
  - `gs_worldmap.c`: 47 active C, 46 active asm
  - `gs_party_access.c`: 0 active C, 44 active asm
- Some files are present but still describe the asm as authoritative, for example:
  - `src/game/people/people_data.c`
  - `src/game/people/people_field.c`
  - `src/dolphin/exi/EXI2.c`

### Build / Tooling Notes

- `configure.py progress` is not a real status tracker yet. It currently reports only the number of source files and explicitly says detailed tracking is TODO.
- `build/GC6E01/base` contains many rebuilt objects, but `build/GC6E01/start.dol` is not present in this checkout.
- `build/GC6E01/obj` still relies heavily on monolithic target objects; only 11 target `.o` files are present under that directory.

## Last 20 Commits: What They Actually Accomplished

### Aggregate

- 18 of the last 20 commits changed code; 2 changed docs only.
- The code commits added 209 exact-match decompilations if their commit messages are taken at face value.
- The strongest recent momentum is in `scene_init.c`, `effect_util.c`, `people_data.c`, `hsd_dobj.c`, `movie.c`, `gs_task.c`, `gs_pcbox.c`, `gs_render.c`, `gs_worldmap.c`, and `gs_title.c`.
- Recent non-count work also mattered:
  - compiler override discovery for `EXI2.c`
  - SDA2/type cleanup in `gs_material.c`
  - several HSD regressions fixed
  - 3 near-miss functions improved but not fully matched

### Commit-by-Commit Summary

| Date | Commit | Summary |
|------|--------|---------|
| 2026-04-01 | `2685605` | `people_data.c`: 20 functions switched to matching C |
| 2026-04-01 | `02745d4` | 5 functions decompiled across Dolphin/FSYS-related files; added `EXI2.c` compiler override |
| 2026-04-01 | `4c496d6` | `scene_init.c`: 7 more functions switched to matching C |
| 2026-04-01 | `775ec8f` | `effect_util.c`: 40 functions switched to matching C |
| 2026-04-01 | `4d044a3` | `gs_title.c`: 7 functions switched to matching C |
| 2026-04-01 | `c19c70e` | `gs_worldmap.c`: 5 functions switched to matching C |
| 2026-04-01 | `37ee804` | `gs_material.c`: fixed 15 SDA2 declarations and improved type accuracy |
| 2026-04-01 | `610a113` | `scene_init.c`: 12 hard functions switched to matching C |
| 2026-04-01 | `2bdc1b0` | Rewrote `docs/matching_guide.md` with current CodeWarrior matching techniques |
| 2026-04-01 | `e997535` | Improved three near-miss functions in `gs_pcbox.c`, `gs_render.c`, and `movie.c` |
| 2026-04-01 | `1a3910b` | Added the current `docs/phase3_plan.md` snapshot |
| 2026-04-01 | `fff692c` | HSD cleanup: 3 functions decompiled, 7 non-matching regressions corrected |
| 2026-04-01 | `1dada5a` | `scene_init.c`: 23 more functions switched to matching C |
| 2026-04-01 | `bcb8cef` | `gs_task.c`: 7 of 12 target functions switched to matching C |
| 2026-03-31 | `c3336dc` | `movie.c`: 13 of 17 functions switched to matching C |
| 2026-03-31 | `f8fb983` | `gs_pcbox.c`: 7 of 16 functions switched to matching C |
| 2026-03-31 | `3f905b0` | `hsd_dobj.c`: 15 of 17 functions switched to matching C |
| 2026-03-31 | `ff4286b` | `scene_init.c`: 23 more functions switched to matching C |
| 2026-03-31 | `782e269` | `scene_init.c`: 11 functions switched to matching C |
| 2026-03-31 | `2428808` | `gs_render.c`: 11 more functions switched to matching C |

## Documentation Validation

| Doc | Verdict | Evidence from repo | High-confidence action |
|-----|---------|--------------------|------------------------|
| `docs/matching_status.md` | Clearly outdated | Claims 23 fully matching modules, including `gs_field_world.c`, `gs_thread.c`, `input.c`, many HSD files, and `people.c`; current tree still shows active asm wrappers in all of those areas | Mark as historical/superseded |
| `docs/phase3_plan.md` | Partially stale | Techniques section is still useful, but the title uses the wrong game ID and the progress table already lags behind current wrapper state (`effect_util.c`, `people_data.c`, `gs_title.c`, `scene_init.c`, etc.) | Keep the techniques, add an audit note, fix the game ID |
| `docs/matching_plan.md` | Clearly outdated | Uses March 22 counts (`~3,000 verified`, `~5,600 remaining`) that are not tied to the current tree and now conflict with the current recovery state | Mark as historical snapshot |
| `docs/pc_port_design.md` | Partially stale | Design content is still useful, but status now says "pre-implementation" even though `src/pcport/` and the CMake scaffold exist; it also says HSD and input are fully decompiled, which the current tree contradicts | Update status wording and fix the HSD/input status claims |
| `docs/matching_guide.md` | Accurate | Aligns with `compile_config.json`, recent commit history, and the actual matching pain points seen in the tree | No change needed |
| `docs/setup.md` | Mostly accurate | Tooling and file paths match this checkout; required tools and original assets are present locally | No change needed |
| `docs/ghidra_pipeline.md` | Accurate | Referenced scripts, build directories, and workflow files exist in the repo | No change needed |
| `docs/codebase_map.md` | Partially stale | Static address analysis is still useful, but its statistics and "priority areas for further analysis" read like a March snapshot and do not reflect the current source coverage | Human review recommended |
| `docs/function_map.md` | Mostly accurate / static | Primarily an address-layout reference, not a live status tracker | Human review only if naming work resumes |

## Inconsistencies Between Docs and Code

- `docs/matching_status.md` presents several modules as 100% matching when the current tree still contains active asm-backed wrappers in those exact files.
- `docs/phase3_plan.md` undercounts several actively advanced files:
  - `effect_util.c` now has 77 active C wrappers, not 37
  - `people_data.c` now has 53 active C wrappers, not 28
  - `gs_title.c` now has 23 active C wrappers, not 16
  - `scene_init.c` is far beyond the listed 180 switched wrappers
- `docs/pc_port_design.md` describes the pcport as pre-implementation, but the repo already has:
  - `src/pcport/` source files
  - a pcport-only `CMakeLists.txt`
  - explicit TODO-stub shim implementations
- The same PC-port doc says HSD and input are already fully decompiled; the current wrapper state contradicts that.

## Concrete High-Confidence Corrections

- Add a new audit document and point stale status docs at it.
- Treat `docs/matching_status.md` as historical unless it is rewritten from current repo metrics.
- Treat `docs/matching_plan.md` as a historical planning note, not live status.
- Keep `docs/phase3_plan.md` for techniques, but not as a live scoreboard.
- Update `docs/pc_port_design.md` to reflect "scaffolding exists, implementation is still stub-heavy" and to stop claiming HSD/input are fully complete.

## Major Blockers

- Exact compiler/version/flag recovery is still a real blocker:
  - the repo now depends on per-file overrides in `compile_config.json`
  - recent work explicitly added an `EXI2.c` override
- SDA/SDA2 typing and section placement are still a recurring blocker for exact matches.
- Large, state-heavy engine files remain wrapper-heavy:
  - `gs_field_world.c`
  - `people_field.c`
  - `gs_thread.c`
  - `effect_visual.c`
  - `ui_core.c`
- The PC-port build is blocked by missing implementation, not by missing design:
  - `src/pcport/*.c` are still skeletons
  - the pcport CMake target does not yet compile any game or HSD translation units

## Recommended Next Milestone

- Decompile the next large field-world tranche in `src/game/gs_field_world.c`.

Reason:

- It is the single largest remaining game translation unit in the current tree by active asm-backed wrappers (161).
- Recent commits already show strong momentum in adjacent field/state modules (`scene_init.c`, `people_data.c`, `gs_worldmap.c`, `gs_title.c`).
- Reducing `gs_field_world.c` will improve the decomp status more than another small-module cleanup.

## Uncertain / Not Claimed

- I did not claim a project-wide "X functions are byte-identical" total from the repo alone because the current tree mixes fully C files, disabled asm fallback blocks, and historical status docs with conflicting numbers.
- I did not claim the native PC-port build is runnable; the scaffold exists, but the repo still shows empty source lists in `CMakeLists.txt` and stub-heavy shim code.

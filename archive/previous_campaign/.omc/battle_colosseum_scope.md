# PC Port Battle Colosseum Scope

Status: scoped after field/player/NPC movement animation and pkx battle-model animation verification.

## Goal

Bring the PC port from menu/title content into a playable Battle Colosseum vertical slice:

- render a battle scene with 2v2 Pokemon placement;
- step real pkx Pokemon model motions and basic trainer/field assets;
- show battle text and command/menu UI;
- accept player input for a deterministic player-vs-CPU turn;
- grow toward authentic Colosseum flow, Shadow Pokemon, snagging, and rewards without blocking on the whole original battle engine.

The first milestone is not "link the full battle engine." The first milestone is a host-driven battle probe that proves real assets, text, input, and animation can run in the current PC runtime.

## Proven Inputs

- `pcport/real_content_host.c` can now load FSYS/HSD scene data, LZSS pkx members, wrapped pkx HSD payloads, and motion banks headlessly.
- Character motion verification is complete: field/player/NPC models classify distinct idle/walk/run motions.
- Pokemon battle-model coverage is complete enough for Battle Colosseum work: 527 pkx rows emit, 526 real animated models have varying motions, and `pkx_egg.fsys :: egg` is a low-motion placeholder rather than a loader miss.
- Existing PC port paths already cover render bootstrap, HSD object rendering, texture/material swizzling, font/text surfaces, pad shim input, and menu/title flow.
- Local mechanics reference exists in `docs/pokemon_colosseum_reference.md`; it summarizes double battles, Shadow Pokemon, snagging, Battle Mode, and waza-viewer/debug leads with external source links.

## Source Map

- `src/game/gs_battle_setup.c`: encounter setup. The large `GSbattleSetup_Main` path selects trainer battle type, arena, Shadow encounter flags, and story/Mt. Battle/Colosseum cases.
- `src/game/battle/battle_main.c`: original fight lifecycle. `battle_FightStart` initializes battle scene and waza data, spawns `battle_MainLoop`, and `battle_FightEnd` releases battle scene objects.
- `src/game/battle/battle_scene.c`: battle camera, Pokemon placement, send-out/encounter sequences, and battle scene animation state machines. Most useful as an RE guide for host placement/camera.
- `src/game/battle/battle_waza.c`: move animation sequence system. It defines particle/model/camera/sound entry types and Pokemon motion start points, but many state machines remain TODO/stubbed.
- `src/game/battle/battle_logic.c`: best immediate logic source. It already contains usable C for turn ordering, type effectiveness, damage, status, AI skeleton, Shadow state, and snagging helpers.
- `src/game/colosseum_battle.c` and `src/game/colosseum_event.c`: high-level story/Colosseum orchestration. Useful for sequencing and data references, but too broad to make the first PC slice depend on them.

## Asset Map

- Pokemon models: `orig/GC6E01/disc/files/pkx_*.fsys`. Use the pkx-aware path proven by the motion batch: count at `+0x0c`, LZSS payload, HSD payload search preferring public `scene_data`, usually wrapper `+0x40`.
- Battle menus: `colosseumbattle_menu.fsys`, `colosseumbattleconnection_menu.fsys`, `toolbattle_menu.fsys`.
- Waza/battle archives: `wzx_battle_name_*.fsys`, `wzx_battle_name_final.fsys`, `wzx_dark_pokemon_battle*.fsys`, and related `wzx_*` files.
- Candidate debug/test lead: `waza_viewer` is documented in the local reference as a move-viewer room and should be traced as a safe move-animation harness.

First inventory checkpoint:

- `pkx_*.fsys`: 527 archives.
- `wzx*.fsys`: 1108 archives.
- `wzx_*_attack.fsys`: 306 archives.
- `wzx_*_damage.fsys`: 280 archives.
- `wzx_*_special.fsys`: 79 archives.
- `wzx_*status*.fsys`: 23 archives.
- `wzx_*ball*.fsys`: 81 archives.
- `wzx_snatch*.fsys`: 54 archives.
- Colosseum/battle-name/intro families (`wzx_colo_*`, `wzx_battle_name*`, `wzx_intro*`): 34 archives.

## Implementation Plan

### Phase 0: Inventory And Probes

Deliverable: a battle asset inventory plus a no-render/headless probe.

- Add a `pcport_battle_probe` entrypoint or env-gated mode.
- Enumerate battle arena, battle menu, waza, trainer, and pkx assets without rendering.
- For selected pkx pairs, load scene data, count motions, and print stable per-frame checksums.
- Identify model placement metadata: root scale, center/bounds, and useful motion IDs for idle/attack/hit/faint.
- Keep probe exe names unique because WSL/Windows interop has stale-lock and launch-hang history.

### Phase 1: Host-Driven Battle Scene

Deliverable: a visible PC battle scene with real Pokemon models animating.

- Build a host-side `BattleSceneHost` state in `pcport_main.c` or a small `pcport/battle_host.c`.
- Place four pkx models in fixed double-battle slots first: player-left, player-right, enemy-left, enemy-right.
- Use a conservative camera derived from `battle_scene.c` grid/camera notes, then refine with captured GC references.
- Step idle/stance motions using the proven HSD animation path. For one-shot Pokemon motions, loop by restarting at end frame only for display testing.
- Draw command/text overlay using existing font/menu surfaces, not original battle state machines yet.

### Phase 2: Battle Text And Menus

Deliverable: player can navigate Fight/Pokemon/Bag/Run-style commands and see battle messages.

- Inventory message banks and battle menu members from `colosseumbattle_menu.fsys`.
- Implement a host command menu with the existing pad shim edge detection.
- Add message queue primitives: enqueue, type out or instant draw, wait for A/timeout.
- Use literal test strings first, then wire real battle text tables once the encoding/member path is identified.

### Phase 3: Deterministic Turn Slice

Deliverable: one scripted 2v2 turn plays through input, move selection, animation, damage text, HP update, and return to command selection.

- Seed four `BattlePokemon` structs directly in host code with stable test species/moves.
- Reuse `battle_logic.c` functions where linkable and dependency-light: turn priority, type matchup, damage, status gates, and simple AI.
- Keep battle state owned by the host slice until original battle_main/colosseum orchestration is better understood.
- Trigger basic attacker motion, target hit/faint motion if available, and battle text around the result.

### Phase 4: Authentic Integration

Deliverable: replace host approximations with original data/flow where they are proven safe.

- Link small matched battle helpers one at a time behind stubs and probes.
- Move from seeded test Pokemon to trainer/Pokemon data loaded through the real tables.
- Trace `GSbattleSetup_Main`, `BattleSequenceStart`, and `battle_FightStart` only after the host vertical slice proves the rendering/input/text surface.
- Add Shadow Pokemon and snagging as a separate slice because they cross battle logic, inventory, party/PC storage, and story flags.

## Risks

- The original battle lifecycle is thread/scheduler-heavy. Host-driven first avoids depending on `battle_MainLoop` and scene-object scheduler behavior before they are ported.
- `battle_scene.c` and `battle_waza.c` contain large TODO state machines. They are excellent RE guides but not ready as linked runtime dependencies.
- Waza move effects include particles, models, camera, and sound. The first slice should animate Pokemon bodies before attempting full move effects.
- Battle text encoding/member layout is not yet confirmed in the PC path.
- Arena/camera placement needs visual validation, not just checksum validation.

## Immediate Work Packet

Build `pcport_battle_probe` with these commands/behaviors:

- `PCPORT_BATTLE_PROBE=1`: headless inventory. Print selected arena/menu/waza/pkx archive presence and pkx motion counts.
- `PCPORT_BATTLE_PROBE=scene`: render a finite frame-cap battle scene with four fixed Pokemon, default camera, and a text box.
- `PCPORT_BATTLE_PROBE=turn`: deterministic one-turn script. Accept debug input injection like the menu path and print selected move/result.

Validation gates:

- Probe links to a fresh unique exe name.
- Headless mode does not touch window/GL/audio.
- Render mode can be launched by native PowerShell and can dump a BMP/PNG frame.
- Initial species pair should use known-good pkx models such as `absol`, `pikachu`, `eifie`, and `blacky`; avoid `egg` except as an explicit placeholder test.
- No changes to the verified motion classifier unless a regression test repeats the 149 character rows and 527 pkx rows.

## Parallel Agent Packets

- Asset tracer: inventory battle arena/menu/waza archive members and identify which HSD publics expose scene data.
- Text tracer: find battle message banks, encoding, and the draw call path behind `fn_80106698`/`fn_80132A38`.
- Waza tracer: use `waza_viewer` and `battle_waza.c` to map move sequence file formats and Pokemon motion IDs.
- Host implementer: add `pcport/battle_host.c` with fixed 2v2 placement, animation stepping, text overlay, and deterministic turn state.
- Decomp integrator: isolate dependency-light functions in `battle_logic.c` that can be linked into the PC port before scheduler integration.

## Phase 0 Implementation Checkpoint

Commit target: minimal headless vertical slice.

- Added `PCPort_BattleProbe(frames)` in `pcport/real_content_host.c`.
- Added standalone `pcport/battle_probe_main.c`.
- Added dedicated `pcport/battle_probe_stubs.c` for unused render-side HSD DObj/object-link references in the headless probe.
- Built fresh exe: `build_pc/pcport_battle_probe_headless_1780675322.exe`.
- Native PowerShell `Start-Process` validation completed with `summary loaded=4/4 confirmed=yes`.
- Default actors: player-left `eifie`, player-right `blacky`, enemy-left `absol`, enemy-right `pikachu`.
- Probe prints fixed 2v2 placement, menu text (`FIGHT  POKEMON  BAG  RUN`), deterministic selected move/target, player attack, enemy damage, enemy attack, player damage, and end-turn HP state.
- Follow-up table/env pass added default actor and move-script tables. Actor archive members can be overridden with `PCPORT_BATTLE_P0`, `PCPORT_BATTLE_P1`, `PCPORT_BATTLE_E0`, and `PCPORT_BATTLE_E1`. Move labels/damage can be overridden with `PCPORT_BATTLE_PLAYER_MOVE`, `PCPORT_BATTLE_PLAYER_DAMAGE`, `PCPORT_BATTLE_ENEMY_MOVE`, and `PCPORT_BATTLE_ENEMY_DAMAGE`.
- Real-ID table pass added trainer IDs, team slots, species IDs, move IDs, and text IDs to the headless probe records. Defaults are Espeon/Eifie species 196 level 25, Umbreon/Blacky species 197 level 26, Absol species 359 level 50, Pikachu species 25 level 50; player move defaults to Swift ID 129, enemy move defaults to Bite ID 44. Move/text IDs can be overridden with `PCPORT_BATTLE_PLAYER_MOVE_ID`, `PCPORT_BATTLE_ENEMY_MOVE_ID`, `PCPORT_BATTLE_PLAYER_TEXT_ID`, and `PCPORT_BATTLE_ENEMY_TEXT_ID`.
- common_rel table pass loads `common.fsys :: pcommon_rel` and prints bounded rows from the actual move, Pokemon stats, trainer, and trainer-Pokemon tables. Validated `common_rel` size is `0x14E5B0`; Swift ID 129 reads table index 128 with power 60, and Bite ID 44 reads table index 43 with power 60. The native PowerShell gate still ends with `summary loaded=4/4 confirmed=yes`.
- Trainer/team derivation now reads the selected common_rel trainer rows before loading actors. Default trainer `0x0001` resolves to trainer-Pokemon rows `8/9` as Zangoose/Zangoose at level 100, and trainer `0x0200` resolves to rows `630/631` as Gokulin/Nukenin at level 50; the enemy move derives from the table as move ID `213`. The probe keeps explicit fallback handling for unmapped species or actor env overrides.
- Trainer row selection can be changed with `PCPORT_BATTLE_PLAYER_TRAINER` and `PCPORT_BATTLE_ENEMY_TRAINER`; numeric env parsing accepts decimal and `0x` hex values.

Validated output highlights:

- Table-derived `player-left zangoose`: 8 motions, 5 varying.
- Table-derived `player-right zangoose`: 8 motions, 5 varying.
- Table-derived `enemy-left gokulin`: 6 motions, 6 varying.
- Table-derived `enemy-right nukenin`: 4 motions, 4 varying.
- Turn stub ends at `playerHP=79 enemyHP=68 next=command-menu`.

# PC Port Batch 5 Tasklist

Tracked critical-path work after Batch 4C and the HSD host-loader takeover.

## Standing Rule - True Port Only

- Do not invent gameplay, animation, script, story, render, or UI behavior to make the PC port appear farther along. If a path cannot be wired from recovered/decompiled project code yet, mark the exact missing symbol/data contract as a decomp target and keep any host harness code clearly labeled as a smoke-test scaffold.
- Runtime PC-port bridges are allowed only when they preserve a recovered function contract over real assets/data. They must not replace unknown original logic with guessed behavior.

## Batch 5A - JObj Graph-Safe Walkers

- [x] Confirm the authoritative PC-port build path is `python tools\pcport_link.py`.
- [x] Identify the tree-recursive JObj walkers that must tolerate shared instance graphs.
- [x] Add host graph-safe walker overrides for animation and removal paths.
- [x] Add graph-safe dirty/flag propagation for shared-instance readiness.
- [x] Relink `build_pc\pcport_bootstrap.exe`.
- [x] Run targeted character-animation and headed field smoke checks.
- [x] Record verification evidence in the tasklist and memory doc.

Verification:

- `python tools\pcport_link.py` -> `compiled 129 objects; 0 failed to compile`, round 2 linked OK with 1708 stubs, rebuilt `build_pc\pcport_bootstrap.exe`.
- `Select-String build_pc\obj\hsd_host.o` confirmed BOOT object symbols for `HSD_JObjSetFlagsAll`, `HSD_JObjClearFlagsAll`, `HSD_JObjSetMtxDirtySub`, `HSD_JObjRemoveAll`, `HSD_JObjRemoveAnimAll`, `HSD_JObjReqAnimAll`, `HSD_JObjAddAnimAll`, and `HSD_JObjAnimAll`.
- `PCPORT_CHARANIM_BANK_PROBE=12 build_pc\pcport_bootstrap.exe` completed successfully; motion bank reported 11 motions and live frame deltas for non-static motions.
- Headed field smoke launched `build_pc\pcport_bootstrap.exe --field`; after 5 seconds the `Pokemon Colosseum PC Bootstrap` window was responding.

## Batch 5B - hsd_jobj Functional Holes

- [x] Decompile or host-bridge the live-impact instance/reference helpers `fn_801A0744` and `fn_801A0D94`.
- [x] Triage `fn_801A0744` and `fn_801A0D94` with DeepSeek V4 Flash.
- [ ] Triage remaining `hsd_jobj` TODO wrappers from `tools/decomp_work/_interesting_reordered.json`.
- [x] Verify that new JObj work changes linked PC-port runtime behavior.
- [x] Route direct `fn_801A0FBC` loader calls through the active PC host loader.

Active lane:

- Worktree: `C:\Users\douglaswhittingham\pkmn-colosseum`
- Branch: `master`
- Worker focus: active host equivalent of `fn_801A0744` / `fn_801A0D94` in `pcport\hsd_host.c`, because the linked PC-port `HSD_JObjLoadJoint` is the BOOT-order host loader.

Lane result:

- No overlay landed; the first `fn_801A0744` / `fn_801A0D94` candidate was rejected as unsafe and reverted in the worktree.
- Blocker: the real Colosseum `.inc` path includes ref-count transitions through `fn_801A0D48`, `fn_801A0D3C`, `fn_801A0CE8`, `fn_801A0C9C`, `fn_801A0C68`, and ID lookup through `fn_8019C128`; the candidate leaned too much on upstream Melee semantics.
- Main-checkout follow-up landed the live-impact host behavior instead of a generated-copy overlay: `pcport\hsd_host.c` now records a descriptor-to-live-JObj map during `HSD_JObjLoadJoint`, skips private child recursion for `JOBJ_INSTANCE` descriptors, and resolves each instance child to the canonical live JObj after all canonical nodes are loaded. This mirrors the practical `fn_801A0744` / `fn_801A0D94` ID-table lookup semantics in the active PC-port path.
- Host ref-count note: the PC loader intentionally does not increment the shared child ref count when wiring an instance child. The graph-safe `HSD_JObjRemoveAll` ownership path visits and destroys each live JObj pointer once; adding the original assembly ref bump here would leave shared children alive after host graph removal.
- `pcport\pcport_main.c` now exposes `--jobj-instance-smoke`, a headless smoke that constructs a root/canonical-child/instance-sibling descriptor graph, loads it through public `HSD_JObjLoadJoint`, requires `instance->child == root->child`, then runs graph-safe flag, animation, and removal walkers.
- Follow-up: `build_pc\bodies\hsd_jobj\fn_801A0744.c` now replaces the generated neutral `FLIP_AS_STUB` body for direct PC-port calls. It builds a descriptor-to-live lookup by walking paired JObj/Joint trees, resolves RObj/DObj refs, and wires `JOBJ_INSTANCE` children to the canonical live JObj. This makes generated `build_pc\gen\hsd\hsd_jobj.c` contain a real typed `void fn_801A0744(HSD_JObj*, HSD_Joint*)` body.
- `pcport\pcport_main.c` now exposes `--jobj-resolve-smoke`, a direct smoke that allocates a live root/canonical-child/instance-sibling JObj graph, calls `fn_801A0744(root, joint)` directly, and requires the instance child to resolve to the canonical live child before running graph-safe walkers.
- Follow-up: `build_pc\bodies\hsd_jobj\fn_801A0FBC.c` now replaces the generated direct-loader wrapper with a small PC overlay that calls the active host `HSD_JObjLoadJoint`. This bypasses the generated `fn_801A1098` method path for direct `gs_material.c` callers while preserving the already-verified host ownership and shared-instance behavior.
- `pcport\pcport_main.c` now exposes `--jobj-load-wrapper-smoke`, which loads a descriptor graph through `fn_801A0FBC` directly, requires instance-child resolution to the canonical live child, and then runs graph-safe flag, animation, and removal walkers.
- Next gameplay-readiness action: move past HSD loader plumbing into a bounded new-game/field progression smoke that exercises player spawn, collision/update, script/event dispatch, and NPC interaction triggers.

Verification:

- `python tools\pcport_link.py` -> `compiled 129 objects; 0 failed to compile`, round 2 linked OK with 1711 stubs, rebuilt `build_pc\pcport_bootstrap.exe`.
- `build_pc\pcport_bootstrap.exe --jobj-load-wrapper-smoke` -> passed with `JObj fn_801A0FBC direct loader wrapper exercised`.
- `build_pc\pcport_bootstrap.exe --jobj-instance-smoke` -> passed with `JObj instance child resolved to canonical live child`.
- `build_pc\pcport_bootstrap.exe --jobj-resolve-smoke` -> passed with `JObj fn_801A0744 direct resolver exercised`.
- `build_pc\pcport_bootstrap.exe --real-material-delta-smoke`
- `build_pc\pcport_bootstrap.exe --real-scene-slice-2-smoke`
- `build_pc\pcport_bootstrap.exe --real-textured-scene-slice-smoke`
- `build_pc\pcport_bootstrap.exe --real-scene-slice-4-smoke`
- `build_pc\pcport_bootstrap.exe --real-tev-scene-slice-3-smoke`
- `PCPORT_CHARANIM_BANK_PROBE=12 build_pc\pcport_bootstrap.exe`
- `PCPORT_MENU_FRAMES=45 build_pc\pcport_bootstrap.exe --field`
- `python tools\test_verify_gate.py` -> 12 passed, 0 failed.

## Batch 5C - hsd_mobj Material Runtime Holes

- [x] Land a generated-copy PC-port overlay for `fn_801A7128`.
- [x] Triage `fn_801A7128` with DeepSeek V4 Flash.
- [x] Host-bridge the TExp helper labels that blocked safe MObj work.
- [x] Restore the six-smoke real TObj/TExp parser verification gate.
- [x] Wire PC-port `hsdMObj.make_texp` and `hsdTObj.make_texp` to conservative host TExp builders.
- [x] Replace the generated PC body for `fn_801A7B24` so it calls the local `load` and `make_texp` method slots correctly.
- [x] Verify the live HSD character tree load path through `HSD_JObjLoadJoint -> HSD_DObjLoadDesc -> fn_801A7B24`.
- [x] Add a targeted real-content material-state delta probe.
- [x] Verify material state changes on real PC-port scene content.
- [x] Replace the generated PC body for `fn_801A6E24` so material setup calls the PC `make_texp` slot with the correct signature.
- [x] Replace the generated PC body for `fn_801A7E84` and route `HSD_MObjAnim` through it so material AObj keys update live MObj state.
- [x] Replace the generated PC body for `fn_801A7D58` so MObj copies allocate independent PC-side material, PE, and TObj-chain storage.
- [x] Retry hsd_jobj `fn_801A0744` / `fn_801A0D94` from the real Colosseum `.inc` sequence.

Active lane:

- Worktree: `C:\Users\douglaswhittingham\pkmn-colosseum`
- Branch: `master`
- Worker focus: `src\hsd\hsd_mobj.c`, PC-port `REPLACE_BODY` overlays for `fn_801A7128`, `fn_801A7B24`, `fn_801A6E24`, `fn_801A7E84`, and `fn_801A7D58`.

Lane result:

- `build_pc\bodies\hsd_mobj\fn_801A7128.c` now lands as a generated-copy-only `REPLACE_BODY` overlay.
- `src\hsd\hsd_mobj.c` now wires the PC-only `hsdMObj.make_texp` slot to `fn_801A7128`; generated `build_pc\gen\hsd\hsd_mobj.c` contains the replacement body and compiled `build_pc\obj\hsd_mobj.o` contains `_fn_801A7128`.
- The overlay follows the Colosseum `.inc` material/lightmap branch shape, uses the host TExp helper bridge for the known label calls, and routes TObj expression building through the local class method when available with a `PCPort_TObjMakeTExp` fallback.
- `pcport\pcport_main.c` now exposes `--real-material-delta-smoke`, which loads a real swizzled MObj descriptor through `HSD_MObjLoadDesc`, rebuilds TExp through the PC `make_texp` slot, calls `HSD_MObjSetAlpha(1.0f)` and `HSD_MObjSetAlpha(0.25f)`, and requires a framebuffer delta on real `menu_bg00` geometry through `fn_800DAD10`.
- `build_pc\bodies\hsd_mobj\fn_801A6E24.c` now replaces the generated material setup path so it builds the temporary TObj chain and calls `make_texp(mobj, tobj_top, &mobj->texp)` instead of casting the method slot to the old `(mobj, tobj, rendermode)` shape.
- `--real-material-delta-smoke` now directly calls `fn_801A6E24(liveMObj)` after live HSD loading and requires a non-null setup-built `mobj->texp`, so the setup overlay is included in the visible-impact gate.
- `build_pc\bodies\hsd_mobj\fn_801A7E84.c` now replaces the empty generated material-animation dispatcher. It follows the Colosseum `.inc` order: ambient RGB, diffuse RGB, inverted alpha, specular RGB, then PE ref0/ref1/dst alpha.
- `pcport\hsd_host.c` now provides a BOOT-order `HSD_MObjAnim` override so MObj AObj keys reach `fn_801A7E84` before the existing TObj animation walk.
- `--real-material-delta-smoke` now verifies the dispatcher on live real content by requiring type 4 to set diffuse red to 31 and type 7 to set alpha to 0.250 before restoring the material for the framebuffer delta check.
- `build_pc\bodies\hsd_mobj\fn_801A7D58.c` now replaces the generated MObj copy helper. The previous generated body called `fn_801BE4CC` as if it cloned TObj chains, but the current PC-linked symbol is an image-format helper; it also called the mis-typed `fn_80193B10` allocator path. The overlay allocates cloned TObj nodes through `HSD_TObjAlloc`, material storage through `HSD_MaterialAlloc`, PE storage through `HSD_MemAlloc`, preserves borrowed image pointers, and avoids aliasing the source MObj's animation object.
- `--real-material-delta-smoke` now calls `fn_801A7D58` against the live swizzled MObj and requires independent `copyMat` / `copyTObj` pointers plus the expected toon render bit before the alpha framebuffer delta check.
- Main checkout added BOOT-order host overrides in `pcport\hsd_host.c` for `fn_801B707C`, `fn_801B6F5C`, `fn_801B6E74`, `fn_801B6CD8`, `fn_801B64EC`, `fn_801B5F08`, `fn_801B5E40`, `fn_801B7C60`, and `fn_801B4300`.
- Main checkout fixed the existing parser gate failure by classifying narrow I8 ramp/mask stages before the generic direct-sample fallback in `pcport\real_content_host.c`.
- Follow-up integrated in main: `pcport\hsd_host.c` now provides conservative `PCPort_MObjMakeTExp` / `PCPort_TObjMakeTExp` builders, `src\hsd\hsd_mobj.c` and `src\hsd\hsd_tobj.c` wire those into the PC-only class init path, and `build_pc\bodies\hsd_mobj\fn_801A7B24.c` corrects the generated MObj loader's local method-slot mapping.
- Remaining MObj blocker: `fn_801A6B8C`'s full release path cannot be safely overlaid until archive-owned versus heap-owned MObj resource ownership is recovered; current live loads borrow material data from swizzled archive storage. `fn_801A6D08` already matches the class release/destroy dispatch shape.
- Next JObj action complete: direct `fn_801A0FBC` loading now routes to the active host loader in the PC generated copy, so `gs_material.c` callers no longer depend on the still-TODO `fn_801A1098` method path.

Verification:

- `python tools\pcport_gen.py --out-dir build_pc\gen src\hsd\hsd_mobj.c` -> generated `hsd_mobj.c` with 7 flipped wrappers and the `fn_801A7128` replacement body.
- `python tools\pcport_link.py` -> `compiled 129 objects; 0 failed to compile`, round 2 linked OK with 1709 stubs, rebuilt `build_pc\pcport_bootstrap.exe`.
- `build_pc\pcport_bootstrap.exe --real-material-delta-smoke` -> live `HSD_MObjLoadDesc`, `fn_801A7E84` material animation dispatch (`animDiffuseR=31`, `animAlpha=0.250`), `fn_801A6E24` setup, `fn_801A7D58` copy helper (`copyMat` / `copyTObj` non-null), and PC `make_texp` rebuilds produced TExp roots; `HSD_MObjSetAlpha(1.0 -> 0.25)` produced `diffPixels=307200` on real `menu_bg00` geometry.
- `build_pc\pcport_bootstrap.exe --real-scene-slice-2-smoke`
- `build_pc\pcport_bootstrap.exe --real-textured-scene-slice-smoke`
- `build_pc\pcport_bootstrap.exe --real-scene-slice-3-smoke`
- `build_pc\pcport_bootstrap.exe --real-scene-slice-4-smoke` -> passed with `kind0=2 stage00=1 stage01=2 stages0=2 kind1=1 stage10=1 stages1=1`.
- `build_pc\pcport_bootstrap.exe --real-tev-scene-slice-3-smoke`
- `build_pc\pcport_bootstrap.exe --real-tev-scene-slice-2-smoke`
- `build_pc\pcport_bootstrap.exe --gsgfx-visible-smoke`
- `PCPORT_MENU_FRAMES=45 build_pc\pcport_bootstrap.exe --field` -> loaded `D1_garage_1F`, 324 collision triangles, rendered 45 frames, exited cleanly.
- `PCPORT_CHARANIM_BANK_PROBE=12 build_pc\pcport_bootstrap.exe` -> loaded `field_common.fsys :: ken_b1`, set up 11 motions, and stepped live HSD animation trees successfully.
- `python tools\test_verify_gate.py` -> 12 passed, 0 failed.
- `git diff --check` -> clean.

## Batch 5D - hsd_cobj Camera Runtime Holes

- [x] Triage remaining CObj runtime gaps after JObj and MObj stabilize.
- [x] Land safe PC-port overlay for viewport rect unpacker `fn_80194400`.
- [ ] Verify camera behavior against headed field captures.

Active lane:

- Worktree: `C:\Users\douglaswhittingham\pkmn-colosseum-wt-cobj`
- Branch: `pcport-5d-cobj`
- Worker focus: `src\hsd\hsd_cobj.c`, small camera/runtime TODO wrappers with visible field impact.

Lane result:

- Integrated `build_pc\bodies\hsd_cobj\fn_80194400.c` via `tools\pcport_stub_tables.json` `replace_body`.
- `fn_801950D0` was triaged but not landed because its common path still depends on unresolved `fn_8019513C`.
- Verification: `python tools\pcport_gen.py --out-dir build_pc\gen src\hsd\hsd_cobj.c`, `python tools\pcport_link.py`, and headed `build_pc\pcport_bootstrap.exe --field` smoke passed.

## Batch 5E - Display/Render Pass Holes

- [x] Triage display pass wrappers in `src\hsd\hsd_displayfunc.c`.
- [ ] Attempt PC-port overlays for `fn_80198038`, `fn_801985E0`, or `fn_80198B20` only if the result is safe enough to compile and review.
- [ ] Re-run headed field capture after any display overlay lands.

Active lane:

- Worktree: `C:\Users\douglaswhittingham\pkmn-colosseum-wt-display`
- Branch: `pcport-5e-display`
- Worker focus: `src\hsd\hsd_displayfunc.c` first; `src\hsd\hsd_pobj_disp.c` only if a smaller safe target is obvious.

Lane result:

- `fn_80198038`, `fn_801985E0`, and `fn_80198B20` were triaged with DeepSeek V4 Flash.
- No overlay landed; all three responses ended `SAFE_OVERLAY: no`.
- Blocker: displayfunc wrappers need recovered constant/flag contracts before a safe PC-port replacement.
- Next display action: retry only `fn_80198B20` after naming `lbl_80478AC0`, `lbl_80478ACC`, `lbl_802746D0`, and the flag-controlled path, or pivot to a smaller `hsd_pobj_disp.c` helper.

## Batch 5F - Story Field Smoke Gate

- [x] Add a bounded smoke for the current New Game -> field path.
- [x] Require menu handoff into Story Mode field walk.
- [x] Require the Outskirt Stand map (`S1_out`) to load with WZX collision.
- [x] Require Wes (`field_common.fsys :: ken_b1`) to load with character animation ready.
- [x] Require player update/movement over a finite frame run.
- [ ] Add script/event/NPC interaction coverage after those systems are linked far enough to run.

Lane result:

- `pcport\pcport_main.c` now exposes `--story-field-smoke`.
- The smoke reuses the real menu New Game handoff path, skips boot movies only for the smoke, enters the walkable Story Mode field, forces a bounded 30-frame autorun/autopan, and asserts map load, collision, spawn, Wes load, character animation readiness, and nonzero movement.
- The first field target is the current New Game spawn map: `orig/GC6E01/disc/files/S1_out.fsys`.
- This is still not a full playtest gate: script/event dispatch, NPC spawn/talk, message boxes, and story progression remain uncovered.

Verification:

- `python tools\pcport_link.py` -> `compiled 129 objects; 0 failed to compile`, round 2 linked OK with 1711 stubs, rebuilt `build_pc\pcport_bootstrap.exe`.
- `build_pc\pcport_bootstrap.exe --story-field-smoke` -> passed with `floor=2`, `colTris=648`, spawn `(47.7,0.0,79.9)`, final `(69.0,0.0,-28.5)`, `frames=30`, `wes=1`, `anim=1`.
- `build_pc\pcport_bootstrap.exe --jobj-load-wrapper-smoke` -> passed.
- `python tools\test_verify_gate.py` -> 12 passed, 0 failed.

## Batch 5G - Expanded Playtest Gates

- [x] Add a room-warp trigger/collision smoke for the existing D1 garage 1F/B1 cluster.
- [x] Exercise live in-process room reload from player crossing a door trigger.
- [x] Exercise live outdoor field warp from Outskirt Stand into the shop interior.
- [x] Add first worldmap asset/handoff smoke coverage.
- [x] Gate host-side worldmap cursor/select/travel-confirm behavior.
- [x] Wire accepted worldmap destinations to verified host floor targets.
- [ ] Recover real story unlock/floor availability data and a collision-backed Phenac target.
- [x] Gate field motion plumbing/facing cases for idle/walk/run while moving north/south/east/west.
- [x] Link real people field-motion action records into the player motion source (`fn_8018F6F4` key lookup plus `fn_8018F4C8` slot mirror).
- [ ] Replace the host idle/walk/run role projection with the true live retail action-state path from `fn_8012CA84` / `fn_8012C660`.
- [x] Gate host-side START-in-field menu open/close behavior.
- [ ] Link START-in-field to the real menu/title/message subsystems instead of the host overlay.
- [x] Add first NPC talk-state coverage through the real `fn_801812E8` people interaction entry.
- [x] Gate visible retail NPC model instancing from real `S1_out` dependency members.
- [x] Gate first compact `S1_out` people/actor placement marker into NPC model placement.
- [x] Gate `fn_8018F730` / `fn_8018F788` / `fn_8018F87C` `0x158` people snapshot size/export/import round-trip from real `S1_out` placement data.
- [x] Gate a PC-host mirror of the `fn_8018E050` / `fn_8018E1C4` script-owned NPC open/setup path from real `S1_out` placement data.
- [x] Add host-side message box/text progression coverage.
- [x] Gate `menu_msgbox` `fn_80056C54` / `fn_80057270` text-context load/access into the visible NPC dialogue smoke.
- [x] Gate `script_callback` `fn_80053778` text-context resolution into the visible NPC dialogue smoke.
- [ ] Link field messages to the real `menu_msgbox` / script-callback state machine.
- [x] Add live field trigger coverage for the Outskirt train-car door -> NPC dialogue -> shop warp path.
- [ ] Replace the host trigger bridge with the real field trigger/event dispatcher.
- [ ] Add story progression state mutation/restore coverage.
- [x] Restore retail-backed title cast, sand animation, and crossing clouds without the removed host-mismatched logo assumptions.
- [x] Fix boot video/audio playback and the Nintendo logo orientation after the title path is audited.

Lane result:

- `pcport\pcport_main.c` now exposes `--field-room-warp-smoke`, `--field-room-reload-smoke`, `--field-world-warp-smoke`, `--field-locomotion-smoke`, `--field-message-smoke`, `--field-npc-talk-smoke`, `--field-npc-model-smoke`, `--field-people-snapshot-smoke`, `--field-npc-open-smoke`, `--field-start-menu-smoke`, `--worldmap-handoff-smoke`, `--worldmap-menu`, and `--worldmap-menu-smoke`.
- The smoke validates the host warp table and trigger lookup for `D1_garage_1F` -> `D1_garage_B1` and `D1_garage_B1` -> `D1_garage_1F`.
- It also loads both rooms' WZX collision meshes directly, proving the target archives and collision data are reachable before we wire live in-process reload into the gameplay loop.
- The live reload smoke now drives the normal walk loop from just inside garage 1F, lets the player enter the 1F door trigger, returns target floor `1`, reloads `D1_garage_B1` in-process, and renders a post-reload frame.
- The world-warp smoke wires the current Story Mode start map (`S1_out`) to `S1_shop_1F`, trips the Outskirt shop trigger through the normal walk loop, reloads the shop scene/collision in-process, and renders a post-reload frame. It now requires all four Outskirt shop trigger samples, covering the original autorun path and the visible train-car doorway/platform threshold, to resolve to the shop.
- The worldmap handoff smoke simulates the field-to-worldmap transition target, loads `world_map.fsys`, selects `move_demo`, and renders the scene through the same field scene bridge. This proves the target asset/render path, not the full `gs_worldmap` cursor/select/travel-confirm state machine yet.
- The worldmap menu smoke drives a host-side worldmap state path over the real `world_map.fsys` render: cursor movement, destination selection, travel confirmation dialog, and accepted travel result. It does not yet run the real `gs_worldmap` state machine, destination availability, or story-gated floor mapping beyond the current host table.
- Accepted worldmap travel now resolves through the host floor table and renders the selected field target after confirmation. `PYRITE TOWN` maps to `M2_out` and is smoke-gated with WZX collision; `AGATE VILLAGE` maps to `M3_out` and is verified as a render/collision target. `PHENAC CITY` remains route-locked because `M1_out` renders but currently has no discovered WZX mesh.
- The locomotion smoke loads `S1_out` and `field_common.fsys :: ken_b1`, reuses the playable walk loop's movement/yaw helpers, and currently separates visible idle/walk/run role cycles from turn-node action slots: straight idle/walk/run use the data-derived `ken_b1` motion-bank roles (`1/5/8`), while visible turn cases come from recovered retail people action record `0x00F30400` through the `fn_8012C660` selector over `fn_8018F4C8` action slots 1-4. This is not yet the true live retail action-state path because object/model selection and exact HSD blend transitions still depend on unresolved stubs.
- The message smoke loads `S1_out` and the animated player, renders a host-owned two-page field message over the live scene, and asserts type-in progression, A-to-fast-forward, A-to-next-page, and final close behavior. Real field message dispatch still needs `menu_msgbox` / script callback integration.
- `fn_801812E8` is now a PC-port C mirror of the 0x190-byte retail people interaction state entry instead of an auto-generated link stub. The first NPC talk smoke host-seeds two `PeopleEntry` records, drives the recovered talk-start and restore branches, renders the field/message path over `S1_out`, and asserts missing-NPC lookup returns 0. Real floor-spawn NPC records and script-owned talk callbacks remain pending.
- The NPC model smoke reads the real `S1_out.fsys :: FSYS` dependency member list, suffix-resolves the truncated `nt_m_b1` token to `agent_m_b1`, loads that retail actor archive from `S1_out.fsys`, and now requires compact actor marker `0x025D0000` at offset `0x2EC`, action `0x01000011`, position `(56.5,0.0,10.6)` before rendering it over live Outskirt collision/scene state. This gates visible retail model instancing plus the first compact script/actor placement marker.
- The people snapshot smoke mirrors retail `fn_8018F730` / `fn_8018F788` / `fn_8018F87C` enough for the PC host people registry: active people count to `N * 0x158`, export `entry+0x20` plus model-state vectors, clear/import the full record, and verify the imported NPC still resolves through `fn_801812E8` and renders at the decoded S1_out position. This gates the `0x158` snapshot format; the open/setup bridge is covered by the next smoke, while real callbacks/story dispatch remain pending.
- The NPC open smoke exports PC-host mirrors for `fn_8018E050` and `fn_8018E1C4`, feeds them the same real `S1_out` compact placement marker, creates the host `PeopleEntry` for group `2` / index `605`, verifies the action word, script token offset, talk-state transition, and decoded position, then renders `agent_m_b1` over Outskirt Stand while a two-page NPC dialogue opens, advances, closes, and records a bounded host-side story/cutscene marker. It now also requires generated bodies for `menu_msgbox` `fn_80056C54` / `fn_80057270` and `script_callback` `fn_80053778`: `fn_80056C54` loads the active `lbl_803A9768` text context, `fn_80057270` returns it, and `fn_80053778` resolves that same context into callback state. This is visible NPC interaction progression plus the first message/script-callback bridge, but the marker is still host-owned until trigger dispatch, story-state mutation, and cutscene dispatcher are linked.
- The playable New Game field loop now prepares the same real `S1_out` NPC placement/model and binds the train-car/shop exit trigger to a visible NPC dialogue event before allowing the normal `S1_out` -> `S1_shop_1F` warp. `S1_out` carries four overlapping shop trigger samples so the visible doorway/platform threshold is covered in addition to the smoke autorun path. The upgraded `--story-field-smoke` proves menu New Game handoff, Outskirt spawn, live door trigger crossing, NPC dialogue open/advance/close, host story/cutscene marker `1/2`, and post-dialogue shop reload. The generic `--field-world-warp-smoke` remains a raw warp check and explicitly bypasses the story event.
- The START menu smoke loads `S1_out`, renders the animated player, opens a host-owned field menu with START, moves the cursor twice, closes it with START, and the live walk loop now pauses movement/triggers while the menu is open. Real field menu behavior remains pending.
- The title default now keeps the legacy host title logo/prompt disabled while loading the retail `title.fsys` `t_vs_*` cast cutouts, the title sky/cloud band, and the sand-wind overlay by default. The incomplete archive Pokemon-logo billboard DObj is suppressed until the full retail logo composition is fixed; `PCPORT_TITLE_HOST_UI=1` remains the opt-in diagnostic path for the old host overlay.
- The boot sequence now supports `PCPORT_BOOT_DUMP_ITEM` alongside `PCPORT_BOOT_DUMP_FRAME`, so Nintendo, The Pokemon Company, and Genius Sonority can be dumped through the actual GL boot presentation path instead of only dumping the first boot item or relying on standalone THP decode.

Current true-runtime player animation blockers:

- `fn_8012C660` now has a PC-generated C body that calls the recovered action-record accessors with the same slot contract as the original assembly, and the PC port now provides decomp-backed definitions for `fn_8018F4C8`, `fn_8018F6F4`, `fn_8018F658`, and `fn_8018F678`.
- The live field player still is not fully on the original runtime path. The remaining decomp/link targets are object resolution (`fn_800F9318`, `fn_8018D998`, `fn_8018D928`), movement/action setters (`fn_8018790C`, `fn_8018805C`, `fn_801885C4`, `fn_801887D8`), and model/material animation helpers (`GSmodelGetAnimFrame`, `fn_800EC4D0`, `fn_800EC578`, `fn_800EC5B8`, `fn_800EC5FC`, `fn_800EC9DC`, `fn_800ECA78`, `fn_800ECCA8`).
- Do not mark the player animation gate complete until those helpers are either linked from recovered C or explicitly replaced by decomp-backed PC overlays that preserve the recovered function contract.
- Boot THP playback now logs audio availability and drains queued waveOut audio for up to one second at natural movie end before closing, avoiding tail truncation without changing skip/window-close teardown.
- The earlier render-reload hang was fixed by explicitly releasing the prior field scene archive/field animation state before loading the next map and by avoiding repeated field-side `GSgfxInit` calls during an in-process room reload.
- `S1_shop_1F` currently renders as a static field target; its ambient field animation setup is skipped until that map's animjoint path is recovered.
- `include\hsd\hsd_debug.h` now uses a `PCPORT`-only unprototyped `__assert()` declaration so generated/decompiled HSD TUs with mixed assert call shapes compile in the host harness without changing the matching build declaration.

Audit correction (2026-06-11):

- Game-code backed enough to keep as completed: HSD JObj/MObj/CObj overlays and visible material deltas, because the PC build calls the recovered symbols or BOOT-order host overrides over real HSD descriptors/assets and gates visible framebuffer or graph behavior.
- Asset/render/collision backed but still host-orchestrated: field scene load, WZX collision load, field character load, room/world reload mechanics, and worldmap render handoff.
- Host scaffolds, not game-code correctness: hand-written field warp exits/spawns, host worldmap menu/destination table, host START field menu overlay, host message box pages, and menu New Game smoke skip/autorun.
- Incorrectly overstated before this audit: `--field-locomotion-smoke` did not prove true directional animation correctness while it treated `fn_8012C660` action slot 1 as straight movement, because `ken_b1`'s visible motion-bank roles are `idle=1`, `walk=5`, `run=8`. The live loop now uses those role cycles for straight/idle movement and reserves recovered action-record slots `2/3/4` for turn cases. Remaining animation gaps are exact HSD blend transitions and replacing the host selector with the real runtime model object.
- Animation fix follow-up (2026-06-12): the live field/player animation evaluator now interprets JObj AObj/RObj motion only through `PCPort_HSDJObjAnimJointOnlyAll`; the full DObj/MObj material animation walk is still unsafe for some swizzled field descriptors and stays out of live player/field ticking until that path is recovered. Host `HSD_DObjLoadDesc` / `HSD_PObjLoadDesc` overrides are present for descriptor loading, but not used to force unsafe material animation during gameplay smokes.
- Incorrectly overstated before this audit: title foreground/camera composition included host-made logo/cast/cloud/pan assumptions. The PC front-end now defaults to the real `title.fsys:logo_demo` archive scene plus the retail-backed 2D cast/cloud/sand overlays that were already recovered; legacy host title logo/prompt/camera diagnostics are opt-in. The partial archive Pokemon-logo billboard is suppressed by default until the full retail logo composition is recovered.

Audit verification (2026-06-12 update):

- `PCPORT_LINK_EXE=build_pc\pcport_bootstrap_storyfix.exe python tools\pcport_link.py` -> `compiled 128 objects; 1 failed to compile: ['hsd_mobj_ext']`, round 2 linked OK with 1729 stubs. The remaining `hsd_mobj_ext` failure is the pre-existing dirty-tree inline-asm issue; `hsd_jobj` now compiles again so character animation is live.
- `build_pc\pcport_bootstrap_storyfix.exe --field-locomotion-smoke` -> passed with `source=record+heuristic-fill`, `key=0x00F30400`, action slots `1:1 2:2 3:3 4:4`, straight role motions `idle=1`, `walk=5`, `run=8`, and turn motions `2/3/4`.
- `build_pc\pcport_bootstrap_storyfix.exe --story-field-smoke` -> passed with New Game -> `S1_out`, door NPC index `605`, story/cutscene marker `1/2`, and warp floor `3` (`S1_shop_1F`).
- `build_pc\pcport_bootstrap_storyfix.exe --field-world-warp-smoke`, `--field-npc-open-smoke`, `--field-message-smoke`, and `--field-start-menu-smoke` -> passed after the live door-story split.
- `PCPORT_LINK_EXE=build_pc\pcport_bootstrap_doorfix.exe python tools\pcport_link.py` -> linked OK with 1687 stubs after widening the dirty-tree generated failures to `hsd_mobj_ext` and `game/gs_field_world`; both failures are outside the scoped PC host door trigger code.
- `build_pc\pcport_bootstrap_doorfix2.exe --field-world-warp-smoke` -> passed with `S1_out` carrying 4 doorway samples, original sample `(69.0,0.0,-30.0)`, far threshold sample `(106.0,0.0,-4.0)` radius `28.0`, target `S1_shop_1F`, shop `tris=196`, `exit->2`, and spawn `(0.0,0.0,35.0)`.
- `build_pc\pcport_bootstrap_doorfix2.exe --story-field-smoke` -> passed with New Game -> `S1_out`, `exits=4`, door NPC index `605`, story/cutscene marker `1/2`, trigger point `(51.5,0.0,21.3)`, and warp floor `3` (`S1_shop_1F`).
- `PCPORT_LINK_EXE=build_pc\pcport_bootstrap_animfix.exe python tools\pcport_link.py` -> `compiled 127 objects; 2 failed to compile: ['hsd_mobj_ext', 'game/gs_field_world']`, round 2 linked OK with 1687 stubs. Both compile failures are known dirty-tree/generated-object failures outside the scoped animation fix.
- `build_pc\pcport_bootstrap_animfix.exe --field-locomotion-smoke` -> passed with `source=record+heuristic-fill`, key `0x00F30400`, straight role motions `idle=1`, `walk=5`, `run=8`, and turn action slots `1:1 2:2 3:3 4:4`; straight walking no longer reuses action slot 1.
- `PCPORT_CHARANIM_BANK_PROBE=40 build_pc\pcport_bootstrap_animfix.exe` -> data-derived locomotion map `idle=1`, `walk=5`, `run=8`.
- `build_pc\pcport_bootstrap_animfix.exe --story-field-smoke` -> passed New Game -> `S1_out`, train-door NPC dialogue/cutscene marker `1/2`, and post-dialogue shop warp floor `3`; this gates the joint-only field/player animation path across longer live field progression.
- `build_pc\pcport_bootstrap_latest.exe` was refreshed to match the animation-fixed build. The default `build_pc\pcport_bootstrap.exe` remained locked by stale Windows `pcport_bootstrap.exe` smoke processes and could not be overwritten from this session.
- `PCPORT_NO_BOOT=1 PCPORT_MENU_FRAMES=1 build_pc\pcport_bootstrap.exe` -> passed and loaded title cast cutouts, drifting-cloud layer, and sand-wind layer while keeping the legacy host title logo/prompt disabled.
- `PCPORT_NO_BOOT=1 PCPORT_MENU_FRAMES=1 PCPORT_ANIM_TIME=0/5 PCPORT_DUMP=... build_pc\pcport_bootstrap.exe` -> title captures verified no mismatched logo, visible cast/cloud/sand, and animation deltas of `sky_0_190=96985`, `sand_196_432=103654`, `fullDiffPixels=200639`.
- `PCPORT_BOOT_DUMP_ITEM=0 PCPORT_BOOT_DUMP_FRAME=0 PCPORT_DUMP=build_pc\boot_item0_nintendo.bmp build_pc\pcport_bootstrap.exe` -> dumped the Nintendo boot item through the GL boot path; visual inspection verified the logo is upright and centered.
- `PCPORT_BOOT_DUMP_ITEM=1 PCPORT_BOOT_DUMP_FRAME=30 PCPORT_DUMP=build_pc\boot_item1_tpc_f30.bmp build_pc\pcport_bootstrap.exe` -> dumped The Pokemon Company THP through the GL boot path; visual inspection verified upright text and the boot path reported `audio enabled (32000 Hz, 2 ch)`.
- `PCPORT_BOOT_DUMP_ITEM=2 PCPORT_BOOT_DUMP_FRAME=48 PCPORT_DUMP=build_pc\boot_item2_gs_f48.bmp build_pc\pcport_bootstrap.exe` -> dumped Genius Sonority THP through the GL boot path; visual inspection verified upright text.
- `PCPORT_THP_FILE=orig/GC6E01/disc/files/movie/tpc.thp PCPORT_THP_OUT=build_pc\thp_tpc_audio.wav build_pc\pcport_bootstrap.exe --thp-audio-smoke` -> passed with `2 ch`, `32000 Hz`, `80152` total samples, `RMS=4948`, and `clip=1.13%`.
- `PCPORT_MENU_FRAMES=1 build_pc\pcport_bootstrap.exe` -> completed normal boot playback for Nintendo logo, TPC THP with audio, GS THP, and the first title frame.
- `build_pc\pcport_bootstrap.exe --field-npc-talk-smoke` -> passed with `fn_801812E8 talk=1 idle=1 restore=1 missing=0`, talk-state `4 -> 0`, restore-state `1/prev=5 -> 5`, `frames=4`, `idleMotion=1`, and live `S1_out` field/message render.
- `build_pc\pcport_bootstrap.exe --field-npc-model-smoke` -> passed with dependency list `nt_m_b1->agent_m_b1,truck_b1->truck_b1,bike_pokemon->bike_pokemon`, rendered `agent_m_b1` from `S1_out.fsys`, `rootJoint=0x50F0`, `npcDrawn=36`, `frames=6`, and decoded compact marker `0x025D0000@0x2EC`, action `0x01000011`, NPC position `(56.5,0.0,10.6)`.
- `build_pc\pcport_bootstrap.exe --field-people-snapshot-smoke` -> passed with `fn_8018F730=344`, `fn_8018F788` / `fn_8018F87C` round-trip, group `2`, index `605`, marker `0x025D0000@0x2EC`, action `0x01000011`, NPC position `(56.5,0.0,10.6)`, `talk=1`, `npcDrawn=24`, and `frames=4`.
- `build_pc\pcport_bootstrap.exe --field-npc-open-smoke` -> passed with `fn_8018E050/E1C4 setup=1`, `fn_80056C54=1`, `fn_80057270=1`, `fn_80053778=1`, group `2`, index `605`, marker `0x025D0000@0x2EC`, action `0x01000011`, NPC position `(56.5,0.0,10.6)`, `talk=1`, `dialogueOpen=1`, `advance=1`, `close=1`, `storyStep=1`, `cutscene=2`, `npcDrawn=120`, and `frames=20`.
- `rg -n "fn_8018E050|fn_8018E1C4|fn_801812E8|fn_8018F730|fn_8018F788|fn_8018F87C" build_pc\gen\pcport_stubs.c` -> no matches after relink; the generated people calls now resolve to the PC C bodies.
- Adjacent `--field-message-smoke`, `--field-start-menu-smoke`, `--field-locomotion-smoke`, and `--story-field-smoke` -> passed after the NPC talk-state change.
- `python tools\test_verify_gate.py` -> 12 passed, 0 failed.
- `git diff --check` -> no whitespace errors in the edited repo files.

Verification:

- `python tools\pcport_link.py` -> `compiled 129 objects; 0 failed to compile`, round 2 linked OK with 1730 stubs, rebuilt `build_pc\pcport_bootstrap.exe`.
- Current NPC-open relink: `python tools\pcport_link.py` -> `compiled 128 objects; 1 failed to compile: ['hsd_mobj_ext']`, round 2 linked OK with 1727 stubs. The compile failure is from pre-existing dirty-tree `src\hsd\hsd_mobj_ext.c` / `include\hsd\hsd_mobj.h` edits outside this scoped NPC gate.
- Current message/script context relink: `python tools\pcport_link.py` -> `compiled 128 objects; 1 failed to compile: ['hsd_mobj_ext']`, round 2 linked OK with 1728 stubs; generated `build_pc\gen\game\menu\menu_msgbox.c` includes the `fn_80056C54` / `fn_80057270` `REPLACE_BODY` bodies and `lbl_803A9768` storage, and generated `build_pc\gen\game\script\script_callback.c` includes the `fn_80053778` `REPLACE_BODY`.
- `build_pc\pcport_bootstrap.exe --field-people-snapshot-smoke` -> passed with `fn_8018F730=344`, `fn_8018F788` / `fn_8018F87C` round-trip, imported S1_out-derived NPC group `2`, index `605`, marker `0x025D0000@0x2EC`, action `0x01000011`, position `(56.5,0.0,10.6)`, `talk=1`, `npcDrawn=24`, and `frames=4`.
- `build_pc\pcport_bootstrap.exe --field-start-menu-smoke` -> passed with `S1_out`, `ken_b1`, START open, two cursor moves, START close, `frames=6`, `idleMotion=1`.
- `build_pc\pcport_bootstrap.exe --field-message-smoke` -> passed with `S1_out`, `ken_b1`, two message pages, one fast-forward, one page advance, one close, `frames=24`, `idleMotion=1`.
- `build_pc\pcport_bootstrap.exe --field-npc-model-smoke` -> passed with dependency-list-backed `agent_m_b1` model load/render over `S1_out`, compact marker `0x025D0000@0x2EC`, action `0x01000011`, NPC position `(56.5,0.0,10.6)`, `npcDrawn=36`, and `frames=6`.
- `build_pc\pcport_bootstrap.exe --field-npc-open-smoke` -> passed with `fn_8018E050/E1C4 setup=1`, `fn_80056C54=1`, `fn_80057270=1`, `fn_80053778=1`, group `2`, index `605`, marker `0x025D0000@0x2EC`, action `0x01000011`, visible `agent_m_b1`, two dialogue pages, `advance=1`, `close=1`, host progression marker `storyStep=1`, host cutscene marker `cutscene=2`, `npcDrawn=120`, and `frames=20`.
- `build_pc\pcport_bootstrap.exe --field-locomotion-smoke` -> passed with `S1_out`, `ken_b1`, `source=game-record`, `key=0x00F30400`, action slots `1:1 2:2 3:3 4:4`, and 6 `fn_8012C660` turn-zone cases.
- `PCPORT_NO_BOOT=1 PCPORT_MENU_FRAMES=1 build_pc\pcport_bootstrap.exe` -> passed with default title cast/cloud/sand overlays active and no legacy host logo/prompt.
- `PCPORT_NO_BOOT=1 PCPORT_MENU_FRAMES=1 PCPORT_ANIM_TIME=0/5 PCPORT_DUMP=... build_pc\pcport_bootstrap.exe` -> title visual captures passed with no mismatched logo and cloud/sand drift pixel delta (`sky=96985`, `sand=103654`, `full=200639`).
- `build_pc\pcport_bootstrap.exe --worldmap-menu-smoke` -> passed with cursor `OUTSKIRT STAND -> PHENAC CITY -> PYRITE TOWN`, travel confirm opened, `YES` accepted, then loaded `M2_out` with `floor=4`, `tris=2125`, `frames=9`.
- `build_pc\pcport_bootstrap.exe --worldmap-handoff-smoke` -> passed with `world_map.fsys`, selected/rendered `move_demo`, `rootJoint=0x20`, `extraModels=2`, `collisionTris=0`.
- `build_pc\pcport_bootstrap_latest.exe` now matches `build_pc\pcport_bootstrap_doorfix2.exe`; the default `pcport_bootstrap.exe` path was left stale/locked by old smoke processes during this run.
- `build_pc\pcport_bootstrap.exe --field-room-reload-smoke` -> passed with player trigger `0->1`, start `(0.0,0.0,35.0)`, trigger hit at `(-0.0,0.0,39.9)`, B1 `tris=222`, `exit->0`, spawn `(0.0,0.0,0.0)`.
- `build_pc\pcport_bootstrap.exe --field-room-warp-smoke` -> passed with `D1_garage_1F` `tris=324` `exit->1`, `D1_garage_B1` `tris=222` `exit->0`.
- `build_pc\pcport_bootstrap.exe --story-field-smoke` -> passed after the world-warp smoke change.
- `python tools\test_verify_gate.py` -> 12 passed, 0 failed.
- `bash tools/decomp_work/build_dol.sh` -> OK, byte-identical `main.dol` (`870e8b9693ca780782d80f22a6a4572d8ba9458f`).
- Current true-port audit relink: `PCPORT_LINK_EXE=build_pc\pcport_bootstrap_runtimepath.exe python tools\pcport_link.py` -> `compiled 128 objects; 1 failed to compile: ['hsd_mobj_ext']`, round 2 linked OK with 1726 stubs, rebuilt `build_pc\pcport_bootstrap_runtimepath.exe`.
- Generated stub audit: `fn_8018F4C8`, `fn_8018F6F4`, `fn_8018F658`, `fn_8018F678`, and `fn_8012C660` are no longer generated stubs; the current player-animation blockers remain stubs as listed above.
- `build_pc\pcport_bootstrap_runtimepath.exe --field-locomotion-smoke` -> passed with `source=record+heuristic-fill`, `key=0x00F30400`, visible roles `1/5/8`, action slots `1:1 2:2 3:3 4:4`, and 6 turn cases. This proves the recovered record/selector is linked for the smoke, not that the full live retail player runtime path is complete.
- `build_pc\pcport_bootstrap_runtimepath.exe --story-field-smoke` -> passed with menu New Game handoff, `S1_out` floor 2 collision, Wes animation ready, train-door NPC dialogue marker, story/cutscene host markers `1/2`, and warp to `S1_shop_1F` floor 3.
- `python tools\test_verify_gate.py` -> 12 passed, 0 failed.
- `git diff --check` -> exit 0; CRLF normalization warnings only for pre-existing edited files.

Integration rules:

- Do not edit `*_fn_*.inc` truth files.
- Do not flip asm wrapper `#if` switches for PC-port-only progress.
- Prefer `build_pc\bodies\<tu>\<fn>.c` plus `tools\pcport_stub_tables.json` `replace_body` entries.
- Main checkout owns final integration, `python tools\pcport_link.py`, headed smoke, and commit/push.

## Later Critical Path

- [ ] `battle_scene.c`: 19 asm functions / about 6.1K instructions remain.
- [ ] Cross-cutting smalls: `effect_util fn_80132A38`, `gs_event_exec.c`, `gs_texture.c`, and `fn_800D3088`.
- [ ] Continue skipping host-irrelevant thread/task and GX-init no-op shims unless the PC port proves they are needed.

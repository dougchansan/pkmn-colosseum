# Pokémon Colosseum PC Port — Session Handoff

**Scope of this doc:** continue ONLY the **PC port** (Ship-of-Harkinian–style native port).
**Do NOT** do any byte-match decompilation / matching-build work in this thread.

Branch: `chore/decomp-tooling-reboot`. Latest port commit: `be92efc7` (and descendants).
Repo: `C:\Users\douglaswhittingham\pkmn-colosseum`. Platform: Windows, PowerShell.

---

## 1. What the port is + current state (all committed + working)

The port (`src/pcport/`) is a host layer that runs the game's **own decompiled C** on
GX→OpenGL/GLFW shims (not emulation). It currently boots to a **static 3D title render**:

- `RunMenuScene` (src/pcport/pcport_main.c) loads `title.fsys:logo_demo`, parses the HSD
  scene, and renders it every frame: the **3D desert/ruins** scene + a 2D overlay (the crisp
  **Pokémon Colosseum logo**, **PRESS START**, **copyright**), with face-normal lighting.
- It is a **render harness**: a capped frame loop that exits on window close / frame cap.
  **No input, no game loop, no audio, no state machine, no save/memory-card.**

**Capabilities that already exist (reuse these):**
- FSYS member load + LZSS: `PCPort_LoadFsysMember` (real_content_host.c). LZSS bounds fix is in.
- HSD archive parse + relocate: `PCPort_HSDArchiveParseBE`; public-symbol resolve.
- Scene-graph render: `RenderJointTree` → the game's own GSgfx draw bridge `fn_800DAD10`.
- GC texture decode (gx_texture.c): RGBA8, RGB5A3, I8, CMPR all real; CI4/CI8 + TLUT done.
  Textures are **modulated by the material diffuse** (texture×diffuse) before upload.
- 2D ortho overlays: `BeginMenuOverlay` + `DrawTexturedScreenRect(tex, sx,sy,sw,sh, u0,v0,u1,v1)`
  (screen-space rect + UV sub-rect; gated lighting off so overlays stay full-bright).
- Camera: `BuildViewMatrixLookAt(eye,interest,up,viewMtx)`; the title uses the end-pose of
  `cam_logo_demo_stop` (eye≈(0,38.9,409.8), interest≈(0,39.65,1.56), fov 45).
- Face-normal directional lighting in the gx_tev GLSL shader, gated by a host
  `lightingEnabled` flag (3D scene on, 2D overlays off).
- Decoded menu/title sprites already exist as PNGs under `build_pc/logo_probe/` (e.g.
  `topmenu/menu_033_hdr_RGBA8_276x574.png` = the Story/Battle/Options panels).

**Env toggles (for testing):** `PCPORT_NO_RENDER_3D=1` (2D sky backdrop instead of 3D scene),
`PCPORT_RENDER_DEBUG=1` (per-PObj `[rjt]` diagnostics), `PCPORT_MENU_FRAMES=N` (frame cap),
`PCPORT_DUMP=path.bmp` (dump the framebuffer to BMP on the last frame),
`PCPORT_CAM_EYE/INT/UP="x,y,z"` (override the camera), `PCPORT_NO_TITLE_CAM=1`,
`PCPORT_MENU_ARCHIVE`/`PCPORT_MENU_MEMBER` (render a different fsys scene member).

---

## 2. Build / run / screenshot (IMPORTANT gotchas)

git + python are **NOT on PATH**. Prepend in every PowerShell call:
```
$env:PATH = "C:\Program Files\Git\cmd;C:\Users\douglaswhittingham\AppData\Local\Programs\Python\Python312;" + $env:PATH
```
python.exe: `C:\Users\douglaswhittingham\AppData\Local\Programs\Python\Python312\python.exe`.

**Build the shim lib** (only needed when a shim file — gx_*.c, *_shim.c, real_content_host.c —
changes): `powershell -File tools\pcport_build.ps1`.
> The final `pcport_bootstrap` CMake LINK step ALWAYS ends with ~35 unresolved
> `hsd_pobj_disp` externals — that is **EXPECTED/pre-existing**, not your bug. It still
> rebuilds `pcport_shim.lib`.

**Link the runnable exe** (the real build): `python tools\pcport_link.py`. Must print
`compiled 37 objects; 0 failed to compile` and `LINKED OK` (it auto-stubs ~187 residual
asm-only fn_/lbl_ symbols). If only `pcport_main.c` changed you can skip `pcport_build.ps1`
and just run `pcport_link.py`.

**Run:** `build_pc\pcport_bootstrap.exe --menu`. To capture a frame:
`$env:PCPORT_DUMP="build_pc\out.bmp"; $env:PCPORT_MENU_FRAMES="3"; build_pc\pcport_bootstrap.exe --menu`
then convert with Pillow to PNG and view it (the assistant can Read PNGs to visually verify).
For a live window the user can watch: set `PCPORT_MENU_FRAMES` high and run in the background.

---

## 3. Integration roadmap — turn the static title into the interactive flow

Target flow: **boot videos → title → PRESS START → save prompt → main menu
(Continue / New Game / Battle Colosseum) → mode**. This roadmap is the synthesis of a
5-agent read-only scoping pass (`wmsr9u7q2`) whose claims were verified against source —
the file:line and fn_ references below are confirmed. The four features are **NOT peers**:
#1 is a foundation the others structurally depend on; #4 is a large isolated project.

### ⭐ The single best FIRST increment (cheapest visible proof of interactivity, ~1 day)
> **✅ DONE (2026-06-01, uncommitted in working tree).** `pad_shim.c`:
> `PADShim_UpdateKeyboard` reads GLFW (`glfwGetKey`), keymap set in `PADShim_Init`
> (Enter=START, Z=A, X=B, arrows=dpad, WASD/IJKL sticks). `pcport_main.c` `RunMenuScene`:
> per-frame `PADRead` → host edge-detector `pressed = held & ~prev`; new `PCPortSceneState`
> enum (TITLE→MAIN_MENU); START swaps the overlay logo+`menu_018` → `menu_033` panel; loop
> un-capped (until `glfwWindowShouldClose`), `PCPORT_MENU_FRAMES`/`PCPORT_DUMP` keep the
> headless path finite + vsync-paced; `PCPORT_DEBUG_START_FRAME=N` injects a one-frame START
> for headless verification. Built clean (shim lib + `pcport_link.py` 37/0/LINKED OK),
> screenshot-verified both states, adversarially reviewed (4 lenses, 0 confirmed defects).
> **Next: (2) main-menu cursor** — see below.

In `pad_shim.c` + `pcport_main.c`, do exactly three things:
1. Implement `PADShim_UpdateKeyboard` (pad_shim.c:293) against **GLFW** (the file is scaffolded
   for SDL2 but the build links GLFW): `glfwGetKey(PCPort_GetHostWindow(), …)` →
   Enter=START(0x1000), Z=A(0x100), X=B(0x200), arrows=dpad(0x1/2/4/8). Key state is already
   fresh — `glfwPollEvents()` runs every frame in `VIWaitForRetrace_PC` (os_shim.c:379).
2. Add a host edge-detector: each frame call the shim `PADRead`, compute `pressed = held & ~prev`.
3. Change `RunMenuScene`'s `for(frame<frameCap)` (pcport_main.c:4930) to loop until
   `glfwWindowShouldClose`; on START-pressed, swap the overlay from `menu_018` → `menu_033`.
This links **no new TUs** and turns the static render into something that reacts to a button
and changes screen — the literal skeleton (`enum` state + input + present loop) #2/#3 plug into.

### (1) Input + hosted game-loop + state machine — THE FOUNDATION (do first). Effort: MEDIUM.
- **Take the HOST-DRIVEN path, NOT engine-hosting.** Refactor `RunMenuScene`→`RunGame(window)`
  with `enum { ST_TITLE, ST_SAVE_PROMPT, ST_MAIN_MENU }`, reuse the existing present skeleton
  (`VIWaitForRetrace_PC`→`ClearBackbuffer`→`GSgfx_BeginFrame`→draw→`GSgfxSwapBuffers(1)`).
  ~200–400 lines in `pcport_main.c` + filling `pad_shim.c`'s GLFW TODOs.
- **Keep `src/dolphin/pad/Pad.c` OUT of the link** — its `PADRead`/`PADInit` collide with the
  shim and (EnabledBits=0 on PC) would shadow it with no-input. `pcport_bootstrap` today links
  only the render bridge + shims (CMakeLists.txt:267-277) — no game-engine TUs. Good; keep it.
- **DEFER the authentic engine-hosted path** (LARGE→VERY-LARGE): linking
  `gs_thread.c`/`gs_task.c`/`gs_title.c`/`main.c`/`input.c` needs a host cooperative-**fibre**
  runtime for `fn_800A263C` (fiber create) + `fn_800F0308` (per-frame vsync yield the whole
  engine blocks on) + host stand-ins for the asm-only SI poll `fn_800AB150`/`fn_800D0F44`. The
  engine is a vsync-tied fibre scheduler with a broad extern closure — not worth it for boot/menu.

### (2) Main menu (Story / Battle / Options) — fold into #1. Effort: SMALL.
> **✅ DONE (2026-06-01, uncommitted).** Composite built to match the real layout (user
> reference screenshot): `menu_033` is a sprite SHEET — STORY card (v 0.00–0.40) blits screen-
> left, BATTLE card (v 0.41–0.86) screen-right, OPTIONS pill (v 0.872–0.949) centre. `menu_032`
> is the chrome sheet → the pointing-hand cursor (u 0.185–0.285, v 0.66–0.99) + the Quit button
> (u 0–0.162, v 0.40–0.74). `kMainMenuItems[]` table holds the 6 items + hand positions; D-pad
> up/down moves `menuCursor` (A logs select, B → title). Added `DrawSolidScreenRect` (GX_PASSCLR)
> for the backdrop — REQUIRED because the game draw path leaves a **green EFB clear** that shows
> through; an opaque quad covers it. `PCPORT_DEBUG_CURSOR=N` pins the cursor for headless capture.
>
> **Polish DONE (2026-06-01):** (a) real **blue-swirl background** = `topmenu.fsys:menu_bg00`
> texture idx 00 (CMPR 640×480 @ archive offset 0x73C0), baked like the title sky and drawn full-
> screen. (b) **Bitmap-font text system** — `tools/pcport_genfont.py` renders Consolas → an ASCII
> alpha atlas header `src/pcport/pcport_font.h`; `pcport_main.c` `EnsureFontAtlas()` uploads it,
> `DrawTextScreen`/`DrawTextWrapped` blit per-glyph quads (MODULATE-tinted). Reusable for the save
> prompt etc. (c) **Bottom description box** (silver panel) shows the selected item's wrapped
> description (per-item strings in `kMainMenuItems[]`). Font is Consolas, NOT the game's typeface,
> and the box is a drawn panel, not the exact game sprite — functional, swap later if desired.

- Assets already decoded: `topmenu.fsys:menu_033` (RGBA8 276×574 = STORY/Continue/New Game,
  BATTLE/Colosseum/Battle Now, OPTIONS), PNG at `build_pc/logo_probe/topmenu/`; menu_016/031/032.
- In ST_MAIN_MENU: draw `menu_033` via `DrawTexturedScreenRect` + a cursor-highlight quad, move
  a cursor index on dpad-pressed, A selects. No engine, no new decode.
- (The scoping agent for this returned null — the parallel[2] StructuredOutput failure — but the
  asset/draw facts are corroborated by the other three results; treat as a small known increment.)

### (3) Post-START save prompt (Yes/No + save-presence check). Effort: MEDIUM (pragmatic).
> **✅ DONE (2026-06-01, uncommitted) as a menu-selection dialog layer.** New
> `PCPORT_SCENE_DIALOG` state: A on a menu item opens a dialog over the dimmed menu.
> `PCPort_SaveExists()` is the host save-presence check (fopen `PCPORT_SAVE_PATH`, default
> `build_pc/colosseum.sav`, env-overridable; no GCI/SHA-1 — returns 0 → "no saved game data",
> matching the game's fallback). CONTINUE/NEW GAME → Yes/No (when a save exists) or info;
> QUIT → Yes actually `glfwSetWindowShouldClose`; Colosseum/BattleNow/Options → info "not yet
> available". D-pad left/right picks Yes/No, A confirms, B cancels → back to menu. Rendered with
> `DrawSolidScreenRect` (dim + panel) + the bitmap-font text. `PCPORT_DEBUG_A_FRAME=N` injects an
> A press for headless capture. **Remaining:** actual save WRITE + loading real game state (no
> game-state layer yet); the real game's dialog sprite/font.

- **The save/card subsystem is a decomp BLACK BOX** — `fn_801E1300/0FB4/1274/1B2C/12A0` and
  `menuCB_SaveLoad.c` (`menuCBBios_SaveDataAvailable`) have **NO C and NO `.s`/`.inc`** in the
  tree. You cannot link or reference it; **reimplement host-side**.
- Pragmatic host module: presence check = `does <save-dir>/colosseum.sav exist?` (stub
  "no save found" first — matches the game's own `GStitle_Init` fallback); write = a stdio blob.
  Skip the GCI/SHA-1 format (save.h: 0x40 header + SHA-1 via fn_801CC380, GC6E/01) unless
  Dolphin cross-compat is a goal.
- UI = a few `DrawTexturedScreenRect` quads (window frame + Yes/No + cursor). Sprites live in
  `save_menu.fsys` + `prog_memcard.fsys` — **not yet probed**; enumerate with the existing
  FSYS+HSD decode tooling first. The real game's dialog (`menu_dialog.c` fn_80059034) is GS
  retained-mode + message-bank text (`fn_8001E224`) — the port has NO text/glyph renderer, so
  bake prompt strings as sprites or add a minimal bitmap font. Don't port `menu_dialog.c`.

### (4) Boot THP videos — LARGE, isolated; its own parallel track. Effort: MEDIUM (video-only).
> **🔬 DECODER DONE + PROVEN IN-ENGINE (2026-06-01, uncommitted).** `src/pcport/thp_player.c`
> (+ vendored `third_party/stb_image.h`) parses the THP container and decodes video frames to
> RGBA. Verified on `movie/gs_logo.thp` (640×480, 130 frames, 29.97fps, video-only): walks ALL
> 130 frames and decodes them; output matches a PIL reference within ±3/channel (IDCT rounding).
> Frame 48 = the "GENIUS SONORITY" logo, decoded by the port's own code.
> **THE KEY GOTCHA:** THP video frames are baseline JPEG (SOI/DQT/SOF0/DHT/SOS/EOI) BUT with the
> standard `FF00` entropy byte-stuffing **OMITTED** (Nintendo space-saver). Stock JPEG decoders
> (stb, libjpeg/PIL) fail "bad huffman code" on ~85% of frames (any literal 0xFF in the scan).
> Fix = `thp_restuff()`: re-insert a 0x00 after every 0xFF in the scan region [post-SOS .. EOI],
> append a fresh EOI, THEN decode with stb. Frame layout: `[nextSize][prevSize][videoSize]
> [JPEG]` (+ one more size per extra component for audio THPs); next frame at `off + curSize`
> (curSize starts = `firstFrameSize`, then = align32(nextSize)). Header: fps@0x10, numFrames@0x14,
> firstFrameSize@0x18, componentDataOffset@0x20 (video w/h at +0x14/+0x18), movieDataOffset@0x28.
> Verify with `pcport_bootstrap.exe --thp-smoke` (env `PCPORT_THP_FILE`/`PCPORT_THP_FRAME`/
> `PCPORT_THP_OUT` → writes a PPM; pure decode, no GL).
> **▶ PLAYBACK DONE (2026-06-01, uncommitted) — the boot videos play before the title.**
> (1) `GXHostUpdateTexObjRGBA8` (gx_shim.c/.h) reuses one GL texture via glTexSubImage2D (no
> per-frame leak). (2) `RunBootSequence(window)` (pcport_main.c, called from RunMenuScene after
> GSgfxInit) plays gs_logo → tpc → openingdemo full-screen, paced to fps via `glfwGetTime`, START/A
> skips a movie, window-close aborts; `PCPORT_NO_BOOT=1` skips, `PCPORT_BOOT_DUMP_FRAME=N` dumps a
> boot frame for headless verify. (3) All 3 movies verified decoding incl. the 2-component audio
> THPs (tpc 75f, openingdemo 1926f/64MB streamed — frame 240 = the real desert-ruins opening
> scene); boot_f48 = GENIUS SONORITY through the GL path. **Video-only/muted** (ADPCM audio is a
> later, separate add). NOTE: openingdemo is ~64s — skip with START/A. The full flow now runs:
> boot videos → title → main menu → dialogs.
>
> **BOOT ORDER (corrected 2026-06-01): Nintendo → Pokémon → Genius Sonority → opening demo.**
> The brand logos live in their own FSYS archives (NOT THP): `nintendo_logo.fsys:logo_nintendo`
> and `genius_logo.fsys:logo_gs` are raw 0x80-header **CMPR 640×480** sprites (header `02 80 01 e0
> 04`; marker 0x04 = CMPR); `pokemon_logo.fsys` is an `ar` of THP-decoder `.o` files (NOT an
> image) so "Pokémon" uses `tpc.thp`. `RunBootSequence` plays a mixed list: a static logo
> (`BootShowLogo` via `LoadFsysSpriteTexObj`, ~2.6s, skippable) then the THP movies (`BootPlayTHP`).
> `LoadFsysSpriteTexObj(archive, member, &tex)` generalises `LoadRawMenuTexObj` (any fsys + CMPR).
> Nintendo logo verified rendering via the GL path. (Genius also has a static `logo_gs`; the
> animated `gs_logo.thp` is used for nicer motion.)
>
> **POLISH ROUND 2 (2026-06-01, uncommitted):**
> - **CMPR decode bug FIXED** (`gx_texture.c` `decode_dxt1_block`): the 4×4 selector word is
>   MSB-first (`2*(15-(y*4+x))`), was read LSB-first → diagonally-flipped blocks (invisible on
>   gradients, speckled the Nintendo logo). Now crisp; improves ALL CMPR textures.
> - **Save prompt** (`PCPORT_SCENE_SAVE_PROMPT`): START on the title now shows the memory-card
>   message ("The Memory Card in Slot A has been read!") over the title before the menu, via the
>   new shared `DrawDialogBox()` (dark teal panel + lighter border + white text, bottom-centre).
> - **Dialog/description styling**: the menu description box + all dialogs now use the dark
>   `DrawDialogBox` style (white text on dark teal), matching the real game.
> - `PCPORT_DEBUG_A_FRAME=N` injects an A press (dismiss save prompt / open a dialog) for headless capture.
>
> **TITLE 3D SCENE + CAST — RESOLVED (2026-06-02, committed).** Two things were wrong:
> (1) `RenderJointTree` read the DObj chain `next` from `dobj+0x00` (=class_name) instead of
> `+0x04` → only the first mesh of each joint's chain drew (`dobjs 22→43`, drawn `11→35`). Fixed
> → the FULL desert ruins (columns/boulders/rock layers) now render. (2) The posed "characters"
> are NOT in `logo_demo` at all and NOT skinned — they're pre-rendered **2D cutouts** the title
> composites over the desert and **cycles** through sets (set 1 = Wes/Rui/Umbreon/Espeon; others
> = starters/legendaries). Cutouts are `title.fsys` `t_vs_*` members (0x80-header RGB5A3); set 1
> = t_vs_c3 Wes / c4 Rui / c2 Umbreon (+ Espeon, which has no disc cutout → bundled as
> `tools/pcport_assets/title_espeon.rgba`, extracted via a Dolphin texture dump). Implemented:
> `kTitleCastSet1/2/3[]` + `kTitleSets[]` registry + idle-cycle timer in `RunMenuScene`
> (`PCPORT_TITLE_SET=N` pins a set, `PCPORT_CYCLE_SECS` overrides interval). Placement extracted
> by template-matching the cutouts against a clean Dolphin F9 frame (build_pc/logo_probe/
> match_cutouts.py CLEAN=1; per-set rects via discover_set.py). Set 0 EXACT; sets 1-2 PROVISIONAL
> (need an F9 shot of each + the missing starters dump-bridged like Espeon). Full title-cast
> investigation in auto-memory `project_pc_port_title_characters`.
> **TITLE AMBIENT ANIMATIONS — DONE (2026-06-02, committed 441ee9a8).** The two drifting
> effects are in: (1) **clouds** drift left across the sky, (2) a subtle **sand-wind** blows
> across the desert. Both are self-contained 2D overlays in `RunMenuScene` (TITLE/SAVE_PROMPT
> only), time-based off `glfwGetTime`. Clouds = the logo_demo sky texture (idx19, archive
> 0x14A8E0) baked GX_REPEAT and U-scrolled, drawn as a sky band (opaque top + feathered bottom
> via the new `DrawTexturedScreenRectA`); the texture is only ~seamless so `MakeSeamlessHoriz`
> rolls columns by W/2 + heals the central seam → tiles cleanly under the scroll (no moving
> vertical seam — confirmed at cu0≈2.7). Sand-wind = a procedural tileable wisp texture
> (`BuildSandWindTexture`, integer-wavenumber sines + vertical envelope) scrolled subtly over
> the desert. Verified vs the Dolphin reference (`build_pc/logo_probe/dolphin_set1_calib.png`):
> clouds now visible+drifting (port previously showed flat blue), wind localized to the desert
> (mean Δ ~2/255, sky Δ 0). Env tuning: `PCPORT_CLOUD_SPEED` (def 0.010), `PCPORT_WIND_SPEED`
> (def 0.060), `PCPORT_CLOUD_H` (def 210), `PCPORT_NO_CLOUDS`, `PCPORT_NO_WIND`. Headless verify
> affordances added: `PCPORT_ANIM_TIME=<s>` pins the anim clock (the headless loop runs ~20k fps
> so wall-clock barely advances — this makes drift deterministic), `PCPORT_DUMP_SEQ=<base>` +
> `PCPORT_DUMP_SEQ_EVERY=N` capture a within-run frame sequence.
> **STILL OPEN (title polish):** refine cycling sets 1-2 to pixel-exact; minor logo-overlay
> size/pos nudge (real logo is a bit bigger/lower than the 115,34,410,170 overlay). The
> opening-demo "doesn't load" report = its authentic ~7s black fade-in intro (it does decode +
> play; frame 240 = real scene).

- **Key discovery: the orchestration + the ENTIRE THP player are already C-active.** Boot order
  in `src/game/movie.c`: `moviePlayGSLogo`(:410)→`moviePlayTPCLogo`(:420)→`moviePlayOpeningDemo`
  (:111); `movieWaitForFinish`(:82) is the per-movie blocking loop. The whole THP player/parser
  is C-active in `src/game/battle/battle_logic.c`: `THPPlayerOpen`=fn_801E189C, task entry
  fn_801E1924, open+parse fn_801E4778, `THPDraw`(YUV→GX-TEV) fn_801E1FF8, audio fn_801E2CA8,
  `THPPlayerGetState`=fn_801E1874. (These are GC-target pseudo-register C, not host-buildable
  as-is.) All 5 THP files present + verified (THP v1.1, 29.97fps): `movie/{gs_logo(silent),
  tpc(audio),openingdemo(64MB,audio),autodemo01(101MB),staffroll(109MB)}.thp`.
- **Take PATH A (host-side decoder), NOT Path B.** New `src/pcport/thp_player.c`:
  `PCPortTHP_Open/GetState/Stop/PumpFrame`. Reuse `dvd_shim.c` to **stream** frames (never load
  whole-file — openingdemo/autodemo/staffroll are 64–109MB). Parse the 0x40 BE header as
  fn_801E4778 does. The ONE genuinely new capability = a **baseline-JPEG/MJPEG video decoder**
  (THP frames are JPEG-derived: entropy-decode + dequant + IDCT → YUV planes); CPU YUV420→RGBA8
  (no host perf constraint); present via the **existing 2D fullscreen-quad path**
  (`DrawTexturedScreenRect`/`GXHostInitTexObjRGBA8`). Drive from a new `RunBootSequence(window)`
  state machine before the title, at 29.97fps with skip-on-key.
- Ship **video-only/muted first** (gs_logo/autodemo/staffroll are silent; opening plays fine
  muted). ADPCM audio (audio_shim.c stubbed) or faithful Path B (run the game's C player through
  GX-TEV + a cooperative task pump — same trap as engine-hosting) pushes it to LARGE.
- Hangs off only the shared quad-present helper + an optional skip-key → **can proceed in
  parallel** with #2/#3 on its own track.

### Dependency graph
`#1 INPUT+LOOP (foundation)` → `#2 MAIN_MENU` → `#3 SAVE_PROMPT`. `#4 THP` is parallel (only
needs the quad-present helper + a skip-key). Engine-hosting is an optional, deferred swap-out
of #1's host state machine — only revisit if the goal expands to authentic in-game scene logic.

---

## 4. Known issues / deferred

- **Desert ruins/pillars/rocks are MISSING on the title (deeper than first thought).**
  Investigated 2026-06-02 (commit b716acbf adds the diagnostics). The visible "desert" is ONLY
  the flat haze ground plane (pobj#31-37, tex `0x293E0` × grey `B3B3B3`) plus one dark angled
  plane. The detailed **sandstone ruin meshes pobj#1-30** (correct CMPR column/rock textures,
  white diffuse, `alpha=1.0`, 100s-1000s of verts) — AND the **sky dome pobj#5** (the 512x256
  cloud texture `0x14A8E0`, which is why the 3D sky reads flat-blue and the 2D cloud overlay was
  needed) — DRAW (counted in `drawn`) but rasterize **no visible pixels**. Ruled out: frustum
  clip (far=32768), lighting (`PCPORT_SCENE_NOLIGHT` full-bright is identical), back-face cull,
  material alpha (all 1.0), texture decode (the same textures decode fine for the 2D probes).
  All of pobj#1-30 report the *same* joint origin `cam=(0,-40,-410)` while the offset planes
  (#33/35/36/37) vary — so the remaining suspect is the **geometry transform / display-list
  replay** (vertices collapsing/degenerate or mis-transformed) in the game-owned draw bridge
  (`fn_800DAD10` / `PCPort_TranslatePObjFromArchiveBE` / `PCPort_TranslateJointChainToMatrixBE`).
  Diagnostics to resume: `PCPORT_RENDER_DEBUG=1` (`[rjt]` per-pobj texOff/dims/fmt/diff/alpha +
  `[cam]` near/far/fov), `PCPORT_ISOLATE_CHARS="lo-hi"` (+`PCPORT_ISOLATE_FLAT`),
  `PCPORT_SCENE_NOLIGHT`. Next probe = dump the view/clip-space vertex bounds of a big detail
  mesh (pobj#17 1599v or #29 802v) to see whether the geometry is degenerate or off-screen.
  **Alternative (pragmatic):** since the whole title is already a 2D composite (cast = 2D
  cutouts, logo/text/clouds = 2D), bake a faithful desert+ruins 2D backdrop like the clouds.
  **Deferred** — a genuine multi-session 3D-geometry debug.
- The title horizon has a slight diagonal (a built-in geometry tilt the game's camera
  compensates for). Minor.
- No audio (audio_shim.c stubbed); no disc mount beyond loading named fsys members.

---

## 5. Constraints (hard rules)

- Edit only `src/pcport/**` (the host port) + build scripts (`tools/pcport_*`) + docs.
- Do **NOT** touch matching-build files: `*_fn_*.inc`, symbols, splits, `objdiff.json`,
  CodeWarrior configs. Do **NOT** flip `#if 0`→`#if 1` on asm wrappers. **No byte-match decomp.**
- The repo-public prep is done: API keys scrubbed from history; decomp.dev report wired
  (`master`, `GC6E01_report` artifact); MIT license. Don't redo those.

---

## 6. Memory / vault

- Auto-memory: `project_pc_port_state.md` is the landmark port note (read it first). Also
  `project_two_axis_metric.md`, `project_security_scrub_and_decompdev.md`.
- Vault session log: `Daily/2026-05-31.md` + `Daily/2026-06-01.md`; `Projects/pkmn-colosseum.md`.

## 7. First action in the fresh session

1. Read this doc + `project_pc_port_state.md` + the current `RunMenuScene` (pcport_main.c:4710,
   loop at :4930) + the 2D primitives (`BeginMenuOverlay`:472, `DrawTexturedScreenRect`:504,
   `LoadRawMenuTexObj`:538) + `pad_shim.c` (`PADShim_UpdateKeyboard`:293).
2. Confirm the build still runs (`pcport_link.py` → `--menu` → `PCPORT_DUMP` BMP).
3. ✅ **DONE** — the ⭐ first increment (GLFW input + edge-detector + un-capped loop +
   START swaps `menu_018`→`menu_033`). See the ✅ note in §3. Working tree, uncommitted.
4. ✅ **DONE** — the main-menu composite + navigable hand cursor (§3 #2). Matches the real
   layout. Working tree, uncommitted.
5. **NEXT options:** (a) main-menu polish — locate the blue-swirl background member + add the
   bottom description box (needs baked strings or a minimal bitmap font, since the port has no
   glyph renderer); (b) the post-START save prompt (§3 #3, host-reimpl — save subsystem is a
   decomp black box); (c) THP boot videos (§3 #4, isolated parallel track). Grow `RunMenuScene`
   into a named `RunGame(window)` state machine as more states (save prompt, modes) land.

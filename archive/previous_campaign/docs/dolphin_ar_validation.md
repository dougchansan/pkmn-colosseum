# Dolphin Action Replay Validation

Archived: 2026-06-04

Scope: Pokemon Colosseum NTSC-U, local disc `orig/GC6E01/game.iso`, Dolphin 2603a.

Related research: [pokemon_colosseum_reference.md](pokemon_colosseum_reference.md)

## Sources

- TCRF Pokemon Colosseum: https://tcrf.net/Pok%C3%A9mon_Colosseum
- gc-forever AR thread by Ralf: https://www.gc-forever.com/forums/viewtopic.php?t=2024
- gc-forever WIIRD/Gecko thread by Ralf: https://www.gc-forever.com/forums/viewtopic.php?t=2152
- Dolphin README command-line reference: https://github.com/dolphin-emu/dolphin
- Dolphin Action Replay parser/decryptor source:
  - https://github.com/dolphin-emu/dolphin/blob/master/Source/Core/Core/ActionReplay.cpp
  - https://github.com/dolphin-emu/dolphin/blob/master/Source/Core/Core/ARDecrypt.cpp
- Dolphin pipe input issue/config sample: https://bugs.dolphin-emu.org/issues/9688

## Local Dolphin Harness Status

Validated:

- `DolphinTool.exe header -i orig\GC6E01\game.iso` identifies the local image as:
  - Internal name: `Pokemon Colosseum`
  - Game ID: `GC6E01`
  - Revision: `0`
  - Region/country: USA
- `Dolphin.exe --version` reports `Dolphin 2603a`.
- Dolphin can boot the local ISO from an isolated user folder:
  - Batch/headless-ish: `Dolphin.exe -u <user> -b -e orig\GC6E01\game.iso -C Dolphin.Core.EnableCheats=True`
  - Headed: `Dolphin.exe -u <user> -e orig\GC6E01\game.iso`
- Headed screenshots were captured externally from the Dolphin window.
- Dolphin reads user-folder `GameSettings\GC6E01.ini` cheat stanzas in the expected format.

Not validated yet:

- Card e Room and bonus-disc room replacement behavior through restart-time raw AR hooks.
- TCRF encrypted AR blocks as direct Dolphin input.
- Dolphin Action Replay hot-loading in an already-running game.
- True US Bonus Disc WISHMKR/METEOR Jirachi transfer behavior, because no local US Bonus Disc image or `/tgc/pokedownload.tgc` payload was found.

Current blocker:

- Synthetic keyboard events did not reach Dolphin's emulated controller.
- Dolphin's pipe input backend did not consume normal Windows files in `User\Pipes`.
- A Git-for-Windows `mkfifo` creates a `.lnk` shim from Windows' point of view, so this Dolphin build did not expose it as a usable pipe device.
- No Dolphin MCP tool is available in this Codex session.

## Dolphin Automation Surface

The useful local command-line surface is:

```text
Dolphin.exe -u <user-folder> -e <game.iso>
Dolphin.exe -u <user-folder> -b -e <game.iso>
Dolphin.exe -u <user-folder> -m <movie.dtm> -e <game.iso>
Dolphin.exe -u <user-folder> -s <state> -e <game.iso>
Dolphin.exe -u <user-folder> -d -l -e <game.iso>
Dolphin.exe -u <user-folder> -C Dolphin.Core.EnableCheats=True -e <game.iso>
```

Practical interpretation:

- `--user` gives us isolated, repeatable Dolphin profiles.
- `--exec` boots the ISO.
- `--batch` can smoke-test boot without a UI.
- `--movie` is the best next path for deterministic controller input.
- `--save_state` can jump into a known state once we have or create one.
- `--debugger` and `--logger` expose Dolphin UI/debug panes, not a stable external MCP/API.

## Encrypted AR Code Validation

Dolphin supports:

- Raw/decrypted Action Replay lines: `XXXXXXXX YYYYYYYY`
- Encrypted lines: `XXXX-XXXX-XXXXX`

Dolphin's current encrypted parser performs a per-line parity check before decryption. I ported the relevant logic from Dolphin's `ARDecrypt.cpp` into an ephemeral local Python run and verified it against a known public sample:

```text
Encrypted:
G12C-TMX0-WRT5C
G2ND-C1RJ-G4TZ1

Decrypted:
01E22DC2 08000000
00690E90 000004FF
```

Result for the TCRF/gc-forever encrypted Colosseum blocks:

- The TCRF USA debug block's first encrypted verifier line passes parity, but the following lines fail Dolphin's current parity check.
- Several older gc-forever encrypted Colosseum examples behave the same way.
- gc-forever discussion explicitly recommends making Dolphin use easier forms: decrypt the AR codes and remove the first identification line, or use the WIIRD version.

Do not treat this as proof that the old encrypted strings are bad for real hardware or old tools. Treat it as a Dolphin 2603a compatibility result: use raw AR or WIIRD/Gecko equivalents for automation.

## USA Debug Mode Raw AR

For the local `GC6E01` ISO, the WIIRD thread gives the debug toggle behavior:

```text
28401C28 00000004
0047B420 00000000
E2000001 80008000
28401C28 00000008
0047B420 00000001
E2000001 80008000
```

The Dolphin-safe raw Action Replay equivalent is:

Runtime validation against an already-running Dolphin process showed the display flag values directly:

```ini
[ActionReplay]
$Debug Instruction Bar Force Show
0047B420 00000001

$Debug Instruction Bar Force Hide
0047B420 00000000

$Draw Debug Process Bar
0447ACF0 00000001

[ActionReplay_Enabled]
$Debug Instruction Bar Force Show
$Draw Debug Process Bar
```

Decoded:

- `0047B420 00000001`: 8-bit write `0x01` to `0x8047B420`, shows the Japanese `GSvtr` instruction bar.
- `0047B420 00000000`: 8-bit write `0x00` to `0x8047B420`, hides that instruction bar.
- `0447ACF0 00000001`: 32-bit write `0x00000001` to `0x8047ACF0`, shows the thin colored process bar at the lower-left of the viewport.

Static validation:

- These raw AR lines are supported by Dolphin's Action Replay interpreter.
- No unsupported master/self-modifying AR code types are used.
- The write targets `0x8047B420` and `0x8047ACF0` are the key decomp leads for visible debug display flags.

Runtime validation:

- On 2026-06-04, the running Dolphin process was already in Story Mode, so the flag effects were validated by finding Dolphin's emulated MEM1 mappings and applying the raw AR writes directly to process memory.
- `0x8047ACF0 = 00000001` made the colored process bar appear.
- `0x8047B420 = 01` made the Japanese `GSvtr` instruction bar appear at the top of the screen.
- Screenshot evidence is under `temp/live_dolphin_validation_shots/`, especially `live_after_debug_flag_01.png`.
- This validates the runtime effect of the raw writes. It does not prove Dolphin hot-loads a newly-created `GC6E01.ini` while a game is already running.

## USA Room-Replacer Raw AR Forms

These are Dolphin-friendly raw AR forms derived from Ralf's room replacer pattern and the WIIRD/TCRF room targets. They are for `GC6E01` only.

Card e Room, replacing Phenac City PC Basement room `0x0077` with Card e Room `0x0080`:

```ini
[ActionReplay]
$Card e Room Replacer USA Raw
04003900 28030077
04003904 40820008
04003908 38600080
0400390C 7C7C1B78
04003910 480FBCA0
040FF5AC 4BF04354

[ActionReplay_Enabled]
$Card e Room Replacer USA Raw
```

Japanese bonus-disc content, replacing room `0x03A1` with `ex_top` room `0x0320`:

```ini
[ActionReplay]
$Bonus Disc Content Replacer USA Raw
04003900 280303A1
04003904 40820008
04003908 38600320
0400390C 7C7C1B78
04003910 480FBCA0
040FF5AC 4BF04354

[ActionReplay_Enabled]
$Bonus Disc Content Replacer USA Raw
```

Static validation:

- All lines are 32-bit raw AR writes.
- The injected code cave is `0x80003900`.
- The hook point is `0x800FF5AC`, which already appears in our offset docs as the room/location loader patch point.

Runtime validation:

- Live direct code writes to `0x80003900` and `0x800FF5AC` were visible in MEM1, but did not affect the already-running Dolphin session. The likely cause is Dolphin JIT cache retention: external `WriteProcessMemory` changes the emulated RAM bytes, but does not invalidate already-compiled translated blocks.
- A restart-safe Dolphin setup was created in the user Dolphin profile by enabling `[Core] EnableCheats = True` and adding the raw Card e Room replacer to `GameSettings/GC6E01.ini`. This still needs restart/boot validation because Dolphin hot-loading of a newly-created per-game INI was not observed.
- Live Card e Room access was validated through the floor state data path instead of a code hook:
  - Starting state in Phenac City PC Basement: `gsFloorCurrentId = 0x00000077`, `gsFloorState = 0x00000002` (running), `gsFloorNextState = 0x00000003`.
  - Wrote `0x80478B18 = 0x00000080` (`gsFloorCurrentId`, target floor) and `0x8047ACD8 = 0x00000003` (`gsFloorState`, unload) while keeping `0x8047ACDC = 0x00000003`.
  - After the floor thread ran, the active context reported `ctx.floorId = 0x00005081`, i.e. floor `0x80 + 0x5001`.
  - `PrintWindow` evidence: `temp/live_dolphin_validation_shots/live_card_e_room_data_warp_printwindow.png`, showing the debug overlay and e-Reader-room terminals/NPCs.
- This validates that Card e Room floor `0x0080` is loadable in the USA build and that a data-only live warp can enter it. It does not validate Dolphin live code-hook patching without JIT invalidation.

## Gecko/WIIRD Caution

The direct WIIRD/Gecko debug toggle crashed local Dolphin 2603a at boot/title with:

```text
IntCPU: Unknown instruction 00000000 at PC = 800018a8
```

That makes direct `[Gecko]` usage a risk for these codes in this setup. For this repo's Dolphin automation, prefer raw `[ActionReplay]` forms unless we specifically validate the Gecko codehandler path.

## Live Bonus Disc Floor And Gift Validation

The standard `GC6E01` game image still contains the Japanese bonus-disc `ex_` rooms. Live floor-state warps validated these room IDs in the running Dolphin process:

| Floor ID | Room | Runtime result |
| --- | --- | --- |
| `0x0320` | `ex_title` / `ex_top` entry | Loads the Japanese expansion-disc title surface. |
| `0x0321` | `ex_top` | Loads the hub with coupon/title and Relic Stone shrine panels. |
| `0x0322` | `ex_coupon` | Loads the Poke Coupon reward menu. |
| `0x0323` | `ex_shrine` | Loads the Relic Stone/Celebi shrine scene. |

Screenshot evidence is under `temp/live_dolphin_validation_shots/`:

- `live_bonus_disc_ex_top.png`
- `live_bonus_disc_floor_0321.png`
- `live_bonus_disc_floor_0322.png`
- `live_bonus_disc_floor_0323.png`

Gift species code points in the USA main DOL:

| Runtime address | Native instruction | Native species | Notes |
| --- | --- | --- | --- |
| `0x801306B4` | `388000FB` | Celebi (`0x00FB`) | `ex_shrine` gift builder, left unchanged for the native Celebi path. |
| `0x801307C4` | `38800019` | Pikachu (`0x0019`) | `ex_coupon` gift builder, patchable for an experimental Jirachi reward. |

Jirachi species is `0x0181`, so `li r4, 0x181` encodes as `38800181`.

On 2026-06-04, live Dolphin PID `44496` was patched in RAM so both paths can be tested from the dormant bonus-disc content:

- Kept `0x801306B4 = 388000FB` for native Celebi.
- Changed `0x801307C4` from `38800019` to `38800181`, converting the native bonus-disc Pikachu gift routine into an experimental Jirachi gift routine.
- Backup of the observed before/after bytes: `temp/live_dolphin_validation_shots/bonus_gift_species_patch_backup.json`.

Dolphin raw AR forms for restart-time testing:

```ini
[ActionReplay]
$Bonus Disc Coupon Gift Becomes Jirachi
041307C4 38800181

$Restore Bonus Disc Coupon Gift Pikachu
041307C4 38800019

$Bonus Disc Shrine Gift Becomes Jirachi
041306B4 38800181

$Restore Bonus Disc Shrine Gift Celebi
041306B4 388000FB
```

Enable only one write for a given address at a time. The restore codes are for reverting the patched instruction, not for simultaneous use with the Jirachi substitutions.

Use the coupon-to-Jirachi patch first when the goal is to test both Pokemon in one session: the shrine remains native Celebi, while the coupon reward path becomes Jirachi. The shrine-to-Jirachi patch is a more invasive substitution because it replaces the native Celebi reward itself.

Limitations:

- This is not the authentic US Bonus Disc WISHMKR Jirachi transfer. It is a main-game dormant bonus-disc reward substitution using species support already present in Colosseum.
- The main game includes `pkx_jirachi.fsys` / `pkx_rare_jirachi.fsys`, but the local workspace does not include the US Bonus Disc image or `pokedownload.tgc`.
- Completing the live reward flow still requires in-game condition checks: the Celebi shrine path expects the Story Mode Shadow Pokemon purification state, and GBA transfer paths expect the corresponding GBA save/link checks.
- External RAM writes may not invalidate Dolphin JIT code that has already been translated. In this session the coupon gift function appeared uncompiled before the patch, but restart-time raw AR remains the cleaner path for repeatable validation.

## Next Validation Path

Best next steps:

1. Restart Dolphin with the user profile and confirm the enabled `GC6E01.ini` raw Card e Room AR code takes effect during a normal transition into floor `0x77`.
2. Restart Dolphin with the raw bonus-disc room replacer and confirm it enters `ex_top` through a normal transition.
3. Validate the live Celebi shrine and coupon/Jirachi gift flows through the in-game menus and record the save/GBA-link preconditions that block or permit each transfer.
4. If restart validation succeeds, keep raw `[ActionReplay]` as the reliable Dolphin path for code patches.
5. If live validation is needed without restart, prefer the floor data path: `0x80478B18` target floor ID plus `0x8047ACD8/0x8047ACDC` state control.
6. Create or obtain a Dolphin movie (`.dtm`) that presses Start, loads the existing save, enters Story Mode, and sends D-Pad Up.
7. Map `0x8047B420`, `0x80401C28`, `0x80003900`, `0x800FF5AC`, `0x80478B18`, `0x8047ACD8`, `0x8047ACDC`, `0x801306B4`, and `0x801307C4` into final local symbols/decomp names.

Fallbacks:

- Use a real or virtual controller driver if available.
- Create a save state after manual navigation, then use `--save_state`.
- Build a dedicated Dolphin debug harness only if movie/save-state automation is insufficient.

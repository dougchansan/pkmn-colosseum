# Playbook — how to drive Pokémon Colosseum with the harness

This is the operating manual for an agent (or person) playing the game through
`ctl.py`. It encodes the input conventions and menu quirks discovered while
playing from boot to the opening cutscene. Pair it with `README.md` (verb
reference) and `states/manifest.json` (savestate library).

## The core loop

```
screenshot  -> read the screen
state       -> read floor id / game state when it matters
decide      -> choose the input
press/stick -> send it
wait        -> let the game animate/transition (60 frames = 1 second)
screenshot  -> confirm the result
```

Always screenshot AFTER a `wait`, not immediately after the input — menus and
dialogs animate over several frames. Default screenshot method (PrintWindow) is
crash-safe; use it for everything.

## Input conventions (important, non-obvious)

- **Menus are driven by the LEFT ANALOG STICK, not the D-pad.** D-pad presses
  (`press Up/Down`) do NOT move menu cursors in the Story-Mode front-end. Use
  `stick`.
- **Stick axis (fixed convention in ctl.py):** floats in `[-1, 1]`,
  `+y = UP`, `-y = DOWN`, `+x = RIGHT`, `-x = LEFT`.
  - Move cursor **down**: `python ctl.py stick 0 -1 --frames 6`
  - Move cursor **up**:   `python ctl.py stick 0 1 --frames 6`
  - One menu step per call (the stick returns to neutral when the command ends).
  - Don't pass bare integers as "small" stick values — `stick 0 1` is the float
    1.0 (full up); a raw byte like `200` (magnitude > 1) is interpreted as a raw
    pad byte instead.
- **A = confirm, B = cancel/back, Start = advance title / open pause.**
  `press A` defaults to a 2-4 frame tap, which is right for menus and text.

## YES / NO prompts — they default to NO

The recurring `YES / NO` confirmation boxes (e.g. "Is it okay to start a new
Story?", "Is WES OK?") open with the cursor on **NO**, even though the pokéball
bullet sits next to YES. To accept:

```
stick 0 1 --frames 6      # move UP to YES
screenshot                 # verify YES is highlighted (brighter + cursor)
press A                    # confirm
```

If you press A without moving up first, you select NO and the prompt closes
back to the previous screen. When in doubt, screenshot before the final A.

## Advancing text / cutscenes

- Dialogue boxes advance with `press A`. For long text, loop
  `press A --frames 2` then `wait 20` rather than one long hold.
- The opening sequence (Snagem hideout / desert) is a multi-minute in-engine
  cutscene; let it run with `wait` and periodic screenshots. Some segments
  accept `press Start` to skip.

## Walking the overworld

- Hold a direction on the stick for movement: `stick 1 0 --frames 30` walks
  right ~half a second; `stick 0 1 --frames 30` walks "up"/away from camera.
- `state` reports `gsFloorCurrentId` — watch it change to confirm a room
  transition (e.g. menu backdrop `0x03A1` -> name entry `0x0390` -> intro
  `0x03A0`).

## Savestate discipline

- `save-state <name>` before anything risky or hard to re-reach; milestones go
  in `states/manifest.json`.
- `launch --load-state <name>` boots straight into a state cold (verifies the
  state survives a fresh process).
- After `load-state`, the first `framedrawn` frame can be garbled — use the
  default window screenshot, which is always clean.

## Known milestone path (boot -> intro)

1. boot, `wait 1800` (title appears ~30s)
2. `press Start` -> "no save file" -> `press A` -> "create save?" -> `press A`
   -> "save created" -> `press A` -> main menu  (state: `main_menu_fresh`)
3. `stick 0 -1` to NEW GAME, `press A` -> YES/NO -> `stick 0 1` (YES) ->
   `press A` -> name entry  (state: `name_entry`)
4. pick a name (e.g. `stick 0 -1` to WES, `press A`) -> "Is WES OK?" ->
   `stick 0 1` (YES) -> `press A` -> opening cutscene  (state: `intro_start`)

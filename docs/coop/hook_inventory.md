# Co-op hook inventory — input subsystem (G1 scope)

Branch: `feature/coop-wes-rui-lan`, based on `origin/master` @ `a10498ea`.

Every address below was read from the project's own disassembly of the retail
DOL (`build/GC6E01/asm/game/input/input.s`), regenerated locally by dtk from
`orig/GC6E01/sys/main.dol`. Facts are tagged:

- **[D]** disassembly-confirmed (ground truth)
- **[S]** source-inferred from candidate C (not proof)
- **[R]** runtime-confirmed (nothing carries this tag yet)

## 0. Baseline (G0) — PASS

| Item | Value |
|---|---|
| Repo | `dougchansan/pkmn-colosseum` |
| Base commit | `a10498ea` (`origin/master`) |
| Original DOL | `orig/GC6E01/sys/main.dol` = `orig/GC6E01/start.dol` |
| Original SHA-1 | `870e8b9693ca780782d80f22a6a4572d8ba9458f` |
| Original MD5 | `c55df119fad43d7d4ee976c2242211cb` |
| Built DOL SHA-1 | `870e8b9693ca780782d80f22a6a4572d8ba9458f` — **matches** |
| Verify step | `ninja` → `CHECK config/GC6E01/build.sha1: OK` |
| Toolchain | ninja 1.13.0, Python 3.12.10, mwcc GC/1.2.5n + GC/1.3 (auto-downloaded) |

Commands actually run:

```
python tools/setup_worker_worktree.py <worktree> --branch feature/coop-wes-rui-lan
python configure.py --no-progress
ninja
```

The matching build is byte-identical to the original DOL, so the canonical
target is intact and any co-op output must be built as a separate artifact.

## 1. Linked-object reality — blocks the architecture doc's premise

`configure.py` defines the link gate explicitly:

```
Matching      = True            # Object matches and should be linked
NonMatching   = False           # Object does not match and should not be linked
CodeCandidate = NonMatching     # Compared by objdiff, but not linked yet
```

`tools/project.py:1180` uses `link_built_obj = obj.completed`, so only
`Matching` objects are substituted into the DOL.

| Source file | Declared | Linked into DOL? |
|---|---|---|
| `src/game/input/input_exact_800F75FC.c` | Matching | **yes** |
| `src/game/input/input_candidate_800F760C.c` | Matching | **yes** |
| `src/game/input/input_exact_800F76E4.c` | Matching | **yes** |
| `src/game/input/input.c` | CodeCandidate | **no** |
| `src/game/hero_move.c` | *not declared at all* | **no** |
| `src/game/field_camera.c` | CodeCandidate | **no** |
| `src/game/fight_menu.c` | CodeCandidate | **no** |
| `src/game/fight_side.c` | *not declared*; only `fight_side_exact_*` units | **no** |

**Consequence:** every function this prototype needs to influence
(`0x800F7758`–`0x800F8268`, all of `hero_move`, `field_camera`, `fight_menu`)
is supplied by the *original* split objects. Editing those `.c` files changes
`objdiff` scores and nothing else — the emitted DOL is unaffected. The co-op
layer must therefore be a **bounded PowerPC binary hook against verified
original bytes**, built as a separate artifact, exactly as
`colosseum_coop_architecture.md` §5 describes as the fallback. It is in fact
the only available path, not a fallback.

## 2. Pad table layout [D]

Base `lbl_80401C10`, **4 entries**, stride **0x6C**. Every accessor inlines the
same 4-way unrolled scan comparing the entry's first word against the caller's
`padId`; on no match it returns a per-function failure value. The table is
keyed by **logical pad ID, not by array index** — `0` means a free slot.

Field offsets recovered from the accessors and from the poll routine:

| Offset | Size | Meaning | Evidence |
|---|---|---|---|
| `+0x00` | u32 | logical pad ID; `0` = free slot | [D] `fn_800F80B0` writes it |
| `+0x0C` | u32 | poll result returned by `fn_800F7F64` | [D] |
| `+0x18` | 12 B | incoming raw snapshot (memcpy source) | [D] |
| `+0x24` | u16 | **current buttons (held)** | [D] |
| `+0x26` | u8 | main stick X, raw | [D] |
| `+0x27` | u8 | main stick Y, raw | [D] |
| `+0x28` | u8 | sub (C) stick X, raw | [D] |
| `+0x29` | u8 | sub (C) stick Y, raw | [D] |
| `+0x30` | u32 | **previous buttons**, zero-extended from `+0x24` | [D] |
| `+0x34`,`0x35`,`0x38`,`0x3C`,`0x48`,`0x4C` | — | main stick processed/filtered | [D] |
| `+0x36`,`0x37`,`0x40`,`0x44`,`0x50`,`0x54` | — | sub stick processed/filtered | [D] |
| `+0x58` | u8 | main stick X, processed | [D] |
| `+0x59` | u8 | main stick Y, processed | [D] |
| `+0x5A` | u8 | sub stick X, processed | [D] |
| `+0x5B` | u8 | sub stick Y, processed | [D] |
| `+0x5C`..`+0x68` | — | rumble state | [D] `fn_800F78A4` |

## 3. Function inventory [D]

All accessors below are **leaf functions**: no stack frame, no `mflr`, clobber
only `r0` and their own scratch, return in `r3`. They are safe to call
synchronously from a hook. None can yield or invoke scripts.

| Address | Size | Signature | Behavior |
|---|---|---|---|
| `0x800F78A4` | 0x7C | `(padId, a, b, c, d)` | rumble setup; writes `+0x5C`=1, `+0x60`=`b*15`, `+0x64`, `+0x68` |
| `0x800F7920` | 0x74 | `(padId, sel) -> u8` | sub stick **Y**: `sel==1` → `+0x5B`, else `+0x29` |
| `0x800F7994` | 0x74 | `(padId, sel) -> u8` | sub stick **X**: `sel==1` → `+0x5A`, else `+0x28` |
| `0x800F7A08` | 0x74 | `(padId, sel) -> u8` | main stick **Y**: `sel==1` → `+0x59`, else `+0x27` |
| `0x800F7A7C` | 0x74 | `(padId, sel) -> u8` | main stick **X**: `sel==1` → `+0x58`, else `+0x26` |
| `0x800F7AF0` | 0x6C | `(padId) -> u32` | **changed** buttons: `u16 +0x24` XOR `u32 +0x30` |
| `0x800F7B5C` | 0x68 | `(padId) -> u32` | **not-held** mask: `NOR(+0x24, +0x24)` |
| `0x800F7BC4` | 0x64 | `(padId) -> u16` | **held** buttons: `+0x24` |
| `0x800F7F64` | 0x14C | `(padId) -> u32` | **per-pad poll/advance** (see §4) |
| `0x800F80B0` | 0x88 | `(padId) -> s32` | **pad registration** (see §5) |
| `0x800F8138` | 0x130 | `(...)` | one-time table init + SDK PAD callback hookup [S] |

### Correction to `colosseum_coop_architecture.md`

The doc reads the `sel` argument as selecting the *sub stick*:

```c
subX = fn_800F7A7C(1, 0);
subY = fn_800F7A08(1, 0);
```

That is wrong. `sel` selects **processed (`+0x58`) vs raw (`+0x26`)** on the
*same* axis. The sub stick lives behind different functions entirely
(`fn_800F7994` / `fn_800F7920`). Any adapter written from the doc's reading
would silently feed raw main-stick values where sub-stick values were intended.

Also note every accessor returns via `lbz` — **zero-extended unsigned**. The
signed stick interpretation the architecture doc asks us to preserve happens in
the *caller*, not here. An adapter must not "helpfully" sign-extend.

## 4. `fn_800F7F64(padId)` — the polling boundary [D]

This is the correct hook point for G1. Per call it:

1. Resolves the entry by logical `padId`; returns `2` if absent.
2. `lhz +0x24` → `stw +0x30` — snapshots current buttons as *previous*.
3. `OSDisableInterrupts` → `fn_800D3094` → `memcpy(entry+0x24, entry+0x18, 12)`
   → `OSRestoreInterrupts` — installs the fresh snapshot under interrupt lock.
4. Calls `fn_800F8654` twice — once for the main stick (`+0x26`/`+0x27`), once
   for the sub stick (`+0x28`/`+0x29`) — producing the filtered values.
5. Calls `fn_800F8A54(entry)`.
6. Returns `lwz +0x0C`.

**This resolves the architecture doc's "do not advance cached input history
twice" warning.** The previous-button slot `+0x30` is *per entry*, and the
routine takes `padId` as its argument. Calling it once for ID 1 and once for ID
2 is correct and is not double-advancing; the hazard is only calling it twice
for the *same* ID in one tick. That materially simplifies the input adapter —
no shadow history is required.

It is **not** a leaf: it has a stack frame, calls `memcpy` and the OS
interrupt pair, and must not be re-entered. A hook must respect that.

## 5. `fn_800F80B0(padId)` — registration, not a read [D]

The architecture doc describes this as "accepts logical IDs 1–4", implying a
read. It is actually the **allocator**: it scans the 4 entries for one whose ID
word is `0`, and if `1 <= padId <= 4` stores `padId` there.

Returns: `0` success, `1` if `padId` outside `[1,4]`, `4` if no free slot.

**This is the lever for G1.** Rui's pad exists only if logical ID 2 has been
registered. The open question — the one thing that must be answered at runtime
before any movement work — is whether story mode calls `fn_800F80B0(2)` at all,
or registers ID 1 only.

## 6. Memory map and overlay reservation [D]

Parsed from the original DOL header (`tools/coop/doldef.py`):

| Region | Range |
|---|---|
| text0 | `0x80003100` – `0x800055E0` |
| text1 | `0x800055E0` – `0x80266360` |
| data0–3 | `0x80266360` – `0x8039A6E0` |
| bss | `0x8039A700` – `0x8047E738` |
| data4 (sdata) | `0x80478820` – `0x80478DC0` (inside the bss span) |
| data5 (sdata2) | `0x8047B6A0` – `0x8047E700` |

Five text and five data section slots are unused, so a new loaded section can
be added without disturbing the existing ones.

`OSInit` (`0x80099A44`) establishes the heap base from **instruction
immediates**, not from a data word:

```
80099B40  lwz   r3, BootInfo->arenaLo (+0x30)
80099B50  bne   .L_80099B60          ; keep BootInfo's value if nonzero
80099B54  lis   r3, __ArenaLo@ha     ; 3C 60 80 49
80099B58  addi  r3, r3, __ArenaLo@l  ; 38 63 67 40  -> 0x80496740
80099B60  bl    OSSetArenaLo
...
80099B74  lwz   r3, BI2DebugFlag     ; second path is debug-only:
80099B7C  beq   .L_80099BA0          ;   requires a debug flag
80099B88  bge   .L_80099BA0          ;   and *BI2DebugFlag < 2
80099B8C  lis   r3, _stack_addr@ha   ; -> (0x8048E738 + 0x1f) & ~0x1f
80099B9C  bl    OSSetArenaLo
```

Cross-checked against `config/GC6E01/ldscript.tpl`, the immediates are
self-consistent: `_stack_end` = bss end `0x8047E738`; `_stack_addr` =
`0x8048E738` (64 KB stack); `_db_stack_addr` = `0x80496738` (32 KB debug
stack); `__ArenaLo` = `(_db_stack_addr + 0x1f) & ~0x1f` = `0x80496740`. ✅
`__ArenaHi` = `0x81700000`.

**Therefore a retail boot has arena base `0x80496740`**, and the second
`OSSetArenaLo` only fires on a debug boot.

The reservation strategy follows directly, and is derived rather than guessed:
place the co-op section **at** `0x80496740` and rewrite the `lis`/`addi` pair
at `0x80099B54` so `__ArenaLo` becomes `0x80498740`. The game's own allocator
then starts above the co-op block and can never hand it out. Current
reservation: `0x80496740` + `0x2000`.

Residual risk: if an apploader ever sets `BootInfo->arenaLo` nonzero, the
patched immediate is bypassed and the reservation does not apply. That is a
runtime check, listed in §7.

## 7. Open items before G1 can be called done

- [x] Choose a hook mechanism and reserve overlay space against the real memory
      map (not by inspection of apparently-unused addresses) — see §6.
- [x] Record original instruction bytes for each chosen hook site — encoded in
      `mods/coop/config/GC6E01-hooks.json` and enforced by the builder.
- [ ] **[R]** Confirm at runtime how many IDs story mode registers, and the
      logical-ID → GameCube-port mapping. Nothing above proves port 2 is polled.
- [ ] **[R]** Confirm `fn_800F7F64` is driven once per pad per tick in story
      mode, and locate its caller(s).
- [ ] **[R]** Confirm `BootInfo->arenaLo` is zero on a real boot, so the arena
      reservation actually takes effect.
- [ ] **[R]** Boot the patched DOL and read `coop_stats`.

## 8. Checkpoint gap

`tools/dolphin_harness/states/` holds `intro_start`, `main_menu_fresh`,
`name_entry`, `outskirt_stand`, `test_title`. **None of these is a checkpoint
where Rui is present and script-free** — Outskirt Stand precedes her entirely.
A disposable save at or after Phenac City must be produced before G2 movement
work or the G5 LAN test can begin.

## 9. LAN test note

Both machines are local and each has a human driver, so this is the
architecture doc's §7 LAN procedure as written, not a WAN substitute. The
Windows side has a bundled Dolphin at `dolphin-2603a-x64/Dolphin-x64/`; the
Linux peer must run a **version-identical** Dolphin build, which that bundle's
`build_info.txt` does not state. Pin the exact version before the G5 attempt.


---

## 10. Runtime results (G1) — payload resident, second pad confirmed

All [R] tags below come from booting the patched image under Dolphin's
scripting fork and reading memory (`tools/coop/runtime/`).

### Boot-path finding: added DOL sections are ignored

The disc boot path does not honour sections added to the DOL header. Three
variants were built and booted; in each the payload was absent from memory
(`0x00000000` from frame 1) while the patched `text1` bytes *were* resident:

| Attempt | Result |
|---|---|
| New text slot (`text2`) at `0x80496740` | not loaded |
| New data slot (`data6`), same address | not loaded |
| Slot aliasing existing in-file bytes (no file growth) | not loaded |
| Bare DOL booted directly (Dolphin's own loader) | **loaded** |

So the loader reads our header well enough to apply `text0`/`text1`, but will
not take an added slot. Relocating the DOL on the disc did not help either.

**Resolution:** write the payload *into* `text1`, over code that never runs at
retail. This also keeps the patched DOL exactly the original size
(3,779,808 bytes), so it splices into the ISO at the original `dol_offset`
and the image stays retail-sized.

Region: `0x800C2EC0` + `0x30C`, displacing `TRK__write_aram` and
`TRK__read_aram`, which are called only from `TRKInterrupt` (the CodeWarrior
debugger interrupt path). `InitMetroTRK_BBA` at `0x800C2E18` is deliberately
excluded — `__start` calls it on every boot. Base is 32-byte aligned because
`mwldeppc` aligns `.text`.

### The second controller is already registered

With the payload resident (`magic` = `COOP`, `payload_entry` = `0x4E800020`),
the pad table at `lbl_80401C10` holds logical IDs **1, 2, 3, 4**. The entry
`+0x0C` status word is the connection state, established differentially by
changing only Dolphin's `SIDevice1`:

| slot | 1 pad configured | 2 pads configured |
|---|---|---|
| id 1 | status 0 | status 0 |
| id 2 | status **3** | status **0** |
| id 3 | status 3 | status 3 |
| id 4 | status 3 | status 3 |

**status 0 = connected, 3 = absent.** [R]

This answers the §5 open question and is better news than expected: story mode
does *not* need to be forced to call `fn_800F80B0(2)`. Rui's pad slot exists
and reports connected as soon as a second controller is present.

### Unresolved: the counters do not add up

`poll_total = 1` and `reg_total = 1` after 3600 frames, yet
`reg_count = [1,1,1,1]` (four distinct per-ID slots each incremented once).
Those cannot all be true — four registrations should leave `reg_total = 4`.

More importantly, **`poll_total = 1` means `fn_800F7F64` is not the per-frame
poll.** It is called once during init. The real per-frame input path is
elsewhere — most likely the SDK PAD callback installed by `fn_800F8138`. The
§4 claim that `fn_800F7F64` is "the polling boundary" is therefore **wrong as
a per-frame statement** and must be corrected once the true cadence is found.

The slot snapshot above is unaffected by this: it is copied straight out of
the game's own pad table and it responds correctly to the controller change.

### Remaining for G1

- [ ] Find the actual per-frame input path and re-point the poll hook at it.
- [ ] Explain or fix the counter inconsistency in `coop_hook_register`.
- [ ] Capture non-zero stick/button values with a human driving both pads;
      every sample so far was at rest (`held=0`, sticks 0), which shows the
      slots but not yet independent input.


---

## 11. The per-frame input path — causally confirmed [R]

```
main (0x800059BC)
  GSgappCreate(1, 0, 0, fn_8000604C)      registers a per-frame task
    -> fn_8000604C            0x8000604C  per-frame pad driver
         for id = 1..4:
           fn_8008ABA0(id)    0x8008ABA0  gate; poll only when it returns 0
           fn_800F7F64(id)    0x80006078 -> 0x800F7F64   per-pad latch
```

`fn_8000604C` is never reached by `bl`; its address is taken in `main` and
handed to `GSgappCreate` with priority 0, which is why a caller search for it
comes up empty.

**Causal proof.** Replacing the single `bl fn_800F7F64` at `0x80006078` with a
`nop` (`48 0F 1E ED` -> `60 00 00 00`) and changing nothing else makes all
input stop reaching the pad table — buttons and sticks alike read 0 in every
phase. Restoring it restores input. So this call is the per-frame path, not
merely a plausible one.

### The gate, `fn_8008ABA0(id)` [D]

Reads `lbl_803FB318[id-1]` (call it A) and `lbl_803FB308[id-1]` (B):

- returns 1 when `A != 0 && B == 0`
- returns 0 otherwise

The driver polls when it returns **0**. Any co-op input work must respect this
gate rather than bypassing it.

### Port and button mapping [R]

Established by injecting input per port via Dolphin's scripting `controller`
module and reading the game's own table — no co-op payload involved, so this
describes the stock game:

| Dolphin port | logical pad ID |
|---|---|
| port 0 | **1** |
| port 1 | **2** |

| Input | `+0x24` bits |
|---|---|
| A | `0x0100` |
| B | `0x0200` |

Stick raw bytes centre at 128; full deflection reads 200 and 56 (+/-72), and
`+0x26` (raw) and `+0x58` (processed) agreed in every sample taken at rest or
at full deflection.

### Independent two-pad input works in the stock game [R]

Driving port 0 with A and port 1 with B in the same frame yields
`id1 held=0x0100` and `id2 held=0x0200` simultaneously. Opposite stick
deflections give `id1 rawx=200` / `id2 rawx=56`. Both pads are latched
independently by the retail code with no patch at all.

## 12. Corrections to earlier sections

- **§10's "the game hangs" reading was wrong.** The patched build and the stock
  build produce identical pad-table behaviour over 1800 frames
  (ids `[1,2,3,4]`, statuses `[0,0,3,3]`). The in-place patch does not break
  the game.

- **§4 stands in substance.** `fn_800F7F64` really is the per-frame latch; what
  was missing was its driver and gate, now documented in §11.

- **The in-game telemetry counters are not trustworthy.** `frame_total`,
  `poll_total` and `reg_total` all stick at 1 while the game demonstrably runs
  and latches input every frame, and `reg_total` contradicts `reg_count`. The
  counters live inside `text1` alongside the payload code, which is the leading
  suspect. This is unresolved and must be fixed before any in-game co-op logic
  relies on payload-resident state.

- **Observation should be script-side.** Reading the game's own structures from
  the Dolphin scripting harness needs no payload, no hooks and no patched DOL,
  and it produced every result in §11. Use it for evidence; reserve the payload
  for changing behaviour.

---

## 13. The recomp: a savestate restores the other build's code [R]

Measured on ModernGekko (not Dolphin), co-op DOL `47f8322f…`, hook-aware
module, probe mod `coop_mod.c` (`diag8`), headless, automation driving
`main_x` in 24-30 frame taps on port 0 then port 1.

### What the field map actually is — re-derived from the shipped DOL

§2's offsets were right, and now have a second, independent derivation that
does not go through Dolphin. `GSinputGetLeftStickXData` (0x800F7A7C) is nine
instructions:

```
800F7A7C  lis   r5, 0x8040
800F7A80  lwzu  r0, 0x1c10(r5)      ; table 0x80401C10
800F7A88  cmpw  r0, r3              ; 4-way scan, stride 0x6C, key +0x00
...
800F7AD8  cmpwi r4, 1
800F7AE0  lbz   r3, 0x58(r5)        ; sel == 1 -> processed
800F7AE8  lbz   r3, 0x26(r5)        ; else      -> raw
```

So the table base, the stride, the key and both stick offsets are readable
straight out of the binary, and `GSinputRead`'s
`memcpy(entry+0x24, entry+0x18, 12)` fixes `+0x18` as the incoming SI snapshot
and `+0x24` as the latched copy. **The map was never the problem.**

### Both logical pads receive input, independently

Driving automation port 0 and then port 1, with the probe logging every change:

| | port 0 tap | port 1 tap |
|---|---|---|
| `id=1` raw `+0x26` | 0 → 56 → 0 → 200 → 0 | unchanged |
| `id=2` raw `+0x26` | unchanged | 0 → 56 → 0 → 200 → 0 |

`gate(id) -> 0 (POLLED)` for all four ids; `GSinputRead` runs ~once per frame
per pad. **Port 0 is logical pad 1 and port 1 is logical pad 2 in the recomp,
exactly as §11 found under Dolphin, and the pad table is live for both.**

**The earlier "`+0x24` and `+0x26/+0x27` never change" reading was wrong.** It
came from sampling with no input actually applied: the taps in that session
landed before the field code began reading, because the harness's "settle"
wait was satisfied instantly by the frame-count jump that `--load-state`
itself causes.

### Rui never moves because the running code is the retail build

The probe counts `heroReadStick` (0x8012E388, same address in both builds) by
its member argument:

```
readStick=[491 0 0 0]      ; 491 calls, every one with member 0
moveLeader=[0 0 0 0]       ; hook at the co-op address 0x8012E7BC never fires
main=0                     ; hook at the co-op address 0x8012EBD8 never fires
```

`heroReadStick` has exactly one caller (`moveLeader`, 0x8012E858) and
`moveLeader` exactly two (both in `heroMoveMain`), so `heroMoveMain` is
running — but its co-op branch is not, and the two hooks placed at the co-op
addresses never fire while the one at the shared address does.

Reading the instruction at the call site settles it. The co-op edit is a single
`bl` and the two builds put it four bytes apart:

```
retail   8012EE28  bl 0x8012DE94    partnerMove   (follower AI)
co-op    8012EE2C  bl 0x8012E7BC    moveLeader    (human control)
```

| boot | word at 0x8012EE28 | word at 0x8012EE2C | variant in RAM |
|---|---|---|---|
| co-op DOL, no savestate | `7F83E378` | `4BFFF991` → 0x8012E7BC | **CO-OP** |
| co-op DOL + `part1-rui-named.sav` | `4BFFF06D` → 0x8012DE94 | `3B9C0001` | **RETAIL** |

A Dolphin savestate carries the whole MEM1 image, `.text` included. Every state
in `states/` was captured on 2026-08-19/20, months before the co-op build
existed, so **loading one replaces the co-op code in RAM with the code of the
build it was captured on.** Rui then gets `partnerMove`, the follower AI, which
is precisely what the position probe shows her doing: her model tracks Wes
(member 1 went from `(0, 0, 313.0)` to `(6.5, 0.03, 323.6)` while Wes walked
`0 → 15.5`).

The same control explains the SMC noise, and reverses §12's last bullet for
this case:

| | SMC chunk hash mismatches | `heroMoveMain` in RAM |
|---|---|---|
| with `part1-rui-named.sav` | **66** | retail |
| no savestate | **0** | co-op |

The mismatches are not a red herring here: they are the savestate's text
disagreeing with the module's, chunk by chunk, which also drops ~66 chunks of
the game onto the interpreter.

### Consequence: the savestates cannot be reused across builds

Restoring `.text` from the DOL after a load does not rescue them. The co-op
edit grows `heroReadStick` by 4 bytes, so **4495 text symbols from
0x8012E7B8 to the end of text1 shift by +4**, and a savestate's data and heap
are full of code pointers from the build that wrote them — `heroMoveMain`
itself is reached through one. A retail-era pointer to 0x8012EBD4 lands on
`moveLeader`'s trailing `blr` in a co-op image.

So a co-op savestate has to be captured on the co-op build. Nothing in
`states/` qualifies and `GC/USA/Card A` is empty, so there is no in-game save
to load either.

### Reproducing

```
# probe mod
cmake --build <modsbuild> --target colosseum_coop_mod
# hook-aware pass needs CO-OP function bounds: the retail CSV has
# moveLeader at 0x8012E7B8 and would reject the hook
python tools/coop/gen_functions_csv.py \
    --base <recomp>/patches/GC6E01-functions.csv > build-coop/GC6E01/functions.csv
python <recomp>/scripts/make-hook-aware-module.py --src <built>/dolrecomp-output/generated \
    --dst <built>/hook-aware/generated --functions build-coop/GC6E01/functions.csv \
    --mods <coop>/mods --idle-continuation 800FEB9C
```

Two harness rules the sessions above cost us:

- **A relative frame wait straddling `--load-state` is not a wait.** The load
  jumps `frame_count` by ~152000, so "wait for +240 frames" returns at once and
  the taps land before the field is reading input. Wait for the jump to be
  behind you first.
- **`commands/` is read with `directory_iterator`, so a half-written file is a
  command.** Stage the file outside the directory and rename it in.

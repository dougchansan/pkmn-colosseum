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

## 6. Open items before G1 can be called done

- [ ] **[R]** Confirm at runtime how many IDs story mode registers, and the
      logical-ID → GameCube-port mapping. Nothing above proves port 2 is polled.
- [ ] **[R]** Confirm `fn_800F7F64` is driven once per pad per tick in story
      mode, and locate its caller(s).
- [ ] Choose a hook mechanism and reserve overlay space against the real memory
      map (not by inspection of apparently-unused addresses).
- [ ] Record original instruction bytes for each chosen hook site.

## 7. Checkpoint gap

`tools/dolphin_harness/states/` holds `intro_start`, `main_menu_fresh`,
`name_entry`, `outskirt_stand`, `test_title`. **None of these is a checkpoint
where Rui is present and script-free** — Outskirt Stand precedes her entirely.
A disposable save at or after Phenac City must be produced before G2 movement
work or the G5 LAN test can begin.

## 8. LAN test note

Both machines are local and each has a human driver, so this is the
architecture doc's §7 LAN procedure as written, not a WAN substitute. The
Windows side has a bundled Dolphin at `dolphin-2603a-x64/Dolphin-x64/`; the
Linux peer must run a **version-identical** Dolphin build, which that bundle's
`build_info.txt` does not state. Pin the exact version before the G5 attempt.

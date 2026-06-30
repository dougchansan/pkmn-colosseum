# Real Content Parser Path

## Goal

Cross the current real-content blocker by exposing the smallest real
archive/content parser path that can resolve one FSYS member into an
`HSD_*Desc` graph or equivalent renderable descriptor, without broad engine
boot.

## Ranked Parser / Resolver Options

1. Expose the smallest asm-backed archive helper pair as a host bridge
   - Best next step.
   - `src/hsd/hsd_wobj.c` already contains C fallbacks for the two key helpers:
     - `fn_80191F64`: archive parse / relocation fixup
     - `fn_80191ECC`: public-symbol lookup by name
   - Those functions are much smaller than inventing a fresh parser layer from
     scratch and are enough to test one real member path honestly.

2. Decompile / host-bridge a broader `HSD_ArchiveParse` surface
   - Larger than option 1.
   - The repo still does not have a full `hsd_archive.*` implementation, so
     broad parser bring-up would pull more unknowns into this step.

3. Prove broader parser bring-up is unavoidable and stop immediately
   - This was the fallback if option 1 failed on a real member.
   - It is still useful as the exact blocker outcome, but only after trying the
     narrow helper bridge first.

## Chosen Path

I chose option 1: a host-side bridge that mirrors the behavior of the asm-backed
archive helper pair in `src/hsd/hsd_wobj.c`, then used it on a real
title/menu-adjacent FSYS member:

- archive: `orig/GC6E01/disc/files/topmenu.fsys`
- member: `menu_bg00`
- public symbol: `scene_data`

Why this was the best gradient increase:

- it targets the exact parser boundary already visible in-repo
- it stays below engine boot
- it uses real repository content from the menu path
- it leaves a maintainable smoke harness behind in `pcport_bootstrap`

## What Changed

`src/pcport/real_content_host.c`

- added a narrow host FSYS member loader
- added host LZSS decompression for the real `LZSS` member header format
- added a host big-endian HSD archive parser that mirrors the archive layout
  implied by `fn_80191F64`
- added public-symbol lookup that mirrors `fn_80191ECC`

`src/pcport/real_content_host.h`

- added the host archive parser interface used by the bootstrap smoke path

`src/pcport/pcport_main.c`

- added `--real-content-parser-smoke`
- added `RunRealContentParserSmoke()`, which:
  - loads `topmenu.fsys:menu_bg00`
  - decompresses it
  - parses it as a real HSD archive
  - resolves the public `scene_data` symbol
  - validates that the resolved root contains archive-internal links
  - verifies that direct host-native HSD consumption is still blocked by layout
    mismatch

`CMakeLists.txt`

- added `src/pcport/real_content_host.c` to `pcport_bootstrap`

## What Was Verified

Verified directly:

1. `topmenu.fsys:menu_bg00` is a real HSD-style archive member.
2. The new host bridge can load and decompress that real member from disk.
3. The host bridge can parse the archive header and relocation/public tables.
4. The host bridge can resolve the real public symbol `scene_data`.
5. The resolved `scene_data` root contains valid archive-internal links:
   - `0x0002DC88`
   - `0x0002DC90`
   - `0x0002DCA8`
   - `0x00000000`
6. The existing render bridge regressions still pass unchanged.

Smoke output:

```text
[pcport_bootstrap] Real HSD archive parsed (orig/GC6E01/disc/files/topmenu.fsys:menu_bg00 public=scene_data offset=0x2DCC4 root=0002DC88,0002DC90,0002DCA8,00000000)
[pcport_bootstrap] Direct host HSD consumption still blocked (sizeof(HSD_PObjDesc)=0x30, serialized GC descriptor layout is 0x18 bytes)
```

## Whether A Real FSYS Member Was Resolved Into Descriptor / Object Form

Yes, but only to the first honest parser boundary.

`menu_bg00` is now resolved into a real archive public root (`scene_data`) via a
host archive bridge that mirrors the original helper path. That is a real
repository-content-backed descriptor root in serialized archive form.

Important boundary:

- this is not yet a host-native `HSD_PObjDesc` / `HSD_DObjDesc` / `HSD_Joint`
  graph
- it is a resolved big-endian 32-bit archive graph root

That distinction is direct, not speculative:

- `include/hsd/hsd_pobj.h` defines `HSD_PObjDesc` using native host pointers
- the smoke run measured `sizeof(HSD_PObjDesc) == 0x30`
- the original serialized GameCube descriptor layout is 0x18 bytes

So the existing host render bridge cannot safely consume the resolved archive
root by direct cast or by calling the current HSD loaders on raw archive bytes.

## Whether The Existing Render Bridge Can Consume It

No.

The existing `fn_800DAD10` / `HSD_PObj` bridge still expects host-native object
state, not a big-endian 32-bit archive graph.

The next exact missing step is a host-side translation layer that converts one
resolved archive public root into host-native `HSD_*Desc` / object structures,
or a narrower loader that materializes just the object slice needed by the
current render path.

## Commands Run

Build:

```powershell
cmd /d /s /c '"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul && "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" --build build_pcbootstrap --target pcport_bootstrap -v'
```

Primary verification:

```powershell
.\build_pcbootstrap\pcport_bootstrap.exe --real-content-parser-smoke
```

Regression checks:

```powershell
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-pobj-smoke
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-scene-like-smoke
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-visible-smoke
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-scissor-retry
```

## Results

The chosen parser bridge succeeded.

What compiled and linked:

- `src/pcport/real_content_host.c`
- the updated `pcport_bootstrap` target

What ran:

- `--real-content-parser-smoke`
- all existing render regressions listed above

What did not happen:

- no real content was rendered through `fn_800DAD10`
- no title/menu/game slice became visible from real repository content

## Exact Remaining Blockers

1. The repo can now resolve a real archive public symbol, but the resolved graph
   is still serialized as big-endian 32-bit archive data.
2. Host-native HSD descriptor structs use 64-bit pointers in this build, so
   direct reuse of raw archive bytes is invalid.
3. The resolved public root is `scene_data`, not yet a narrowed `HSD_PObjDesc`
   or other object slice already consumable by the current render bridge.
4. The current bridge still lacks the translation / loader step that turns one
   resolved archive public root into host-native `HSD_*Desc` / `HSD_PObj`
   structures.

## Is Engine Boot Smaller Now?

No.

This is still an inference from the inspected repo paths plus the new parser
result:

- engine boot would now reach the same parser boundary more honestly
- but it would still stop on the same host-native descriptor translation gap
- so engine boot is still larger than crossing that translation layer directly

## Next Smallest Milestone

The next smallest milestone is:

1. identify the narrowest real object slice reachable from `scene_data`
2. translate exactly that one serialized HSD descriptor slice into host-native
   structures
3. feed that translated object into the existing `fn_800DAD10` path

Until that translation step exists, the render bridge still cannot consume real
repository content even though the archive parser boundary is now crossed.

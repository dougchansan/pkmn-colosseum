# Real Content Render Bridge

## Goal

Advance from the real repository-object-backed smoke path to the first real
repository-content-backed object render path, without jumping to engine boot
unless it proves smaller.

## Ranked Options

1. Minimal real FSYS member -> HSD descriptor parse path
   - Best next step.
   - The current bridge already proves that `fn_800DAD10`, the narrow GX
     display-list subset, and a real `HSD_PObj` / `HSD_VtxDescList` path can
     reach the framebuffer.
   - The remaining gap is not assets on disk; it is converting raw repository
     content into the `HSD_*Desc` / `HSD_PObj` structures that the current
     bridge can already consume.

2. Directly wire one decompressed repository blob if it already matches the
   current narrow object/display-list subset
   - Worth checking because it would be smaller than bringing up a parser.
   - I inspected real members from `topmenu.fsys`, `title.fsys`, and
     `genius_logo.fsys` first for exactly this reason.
   - This path failed the inspection step: the real blobs are not already in
     the same form as the current bootstrap-authored `HSD_PObj` contents.

3. Jump to engine boot or broader GX work
   - Larger than option 1 with the current repo state.
   - Even if engine boot reached title/menu init, it would still need the same
     content-resolution layer to turn raw archive members into renderable HSD
     data.

## Chosen Path

I chose option 1: inspect the smallest real FSYS member -> HSD descriptor path,
then stop if the current repository does not yet contain the parser/resolver
needed to feed real content into the already verified `fn_800DAD10` bridge.

Why this was the fastest honest path toward a title/menu-capable slice:

- it reuses the existing visible object/render bridge instead of replacing it
- it directly tests the real remaining gap: repository content resolution
- it avoids broad engine boot if the same missing parser would still block it
- it keeps the boundary explicit between:
  - content that exists in the repo, and
  - content that can actually be rendered by the current native bridge

## What I Inspected

### 1. HSD loader boundary

I searched `src/`, `include/`, `asm/`, and `docs/` for:

- `HSD_ArchiveParse`
- `HSD_Archive`
- `ArchiveParse`
- `HSD_PObjLoadDesc`
- `HSD_DObjLoadDesc`
- `HSD_JObjLoadDesc`

What the repo currently shows:

- `include/hsd/hsd_forward.h` only forward-declares `HSD_Archive`
- no `hsd_archive.*` implementation exists in `src/hsd/`
- `docs/codebase_map.md` includes the original-binary string
  `"HSD_ArchiveParse: byte-order mismatch! Please check data format"`
- `src/hsd/hsd_pobj.c`, `src/hsd/hsd_dobj.c`, `src/hsd/hsd_tobj.c`,
  `src/hsd/hsd_wobj.c`, and related files expose descriptor loaders like
  `HSD_PObjLoadDesc`, `HSD_DObjLoadDesc`, and `HSD_TObjLoadDesc`
- those loaders expect already-materialized `HSD_*Desc*` graphs; they do not
  parse raw archive/member bytes

This is the strongest repo-backed signal that the current missing layer is
archive/content parsing, not object rendering.

### 2. Real repository content on disk

The repo already contains extracted disc content under:

- `orig/GC6E01/disc/files/topmenu.fsys`
- `orig/GC6E01/disc/files/title.fsys`
- `orig/GC6E01/disc/files/genius_logo.fsys`

I parsed the FSYS member tables and decompressed representative members using
the in-repo FSYS layout and the LZSS rules from `src/game/fsys/fsys_decomp.c`.

Representative real members inspected:

| Archive | Member | Group | Decompressed size | Inspection result |
|---|---|---:|---:|---|
| `topmenu.fsys` | `menu_bg00` | `0x2` | `0x10532` | structured binary blob, not a raw GX display list |
| `title.fsys` | `menu_title` | `0x7` | `0x5F` | tiny structured blob, not a direct `HSD_PObjDesc` candidate |
| `genius_logo.fsys` | `logo_gs` | `0x9` | `0x51E7` | structured blob, not already usable by the current object bridge |

The decompressed blob heads were:

- `topmenu.fsys:menu_bg00`
  - `00 02 DE 87 00 02 DC B4 00 00 00 68 00 00 00 01`
- `title.fsys:menu_title`
  - `02 01 01 01 00 02 00 00 00 1C 00 00 00 00 00 20`
- `genius_logo.fsys:logo_gs`
  - `02 80 01 E0 04 01 00 00 00 00 00 B0 00 00 00 00`

Directly verified:

- the real content is present in the repo
- it can be located by name inside FSYS archives
- it can be decompressed using the in-repo FSYS/LZSS rules

Inferred from the blob inspection:

- these members are serialized structured content, not already the narrow
  `HSD_PObj` / raw GX display-list form used by the current smoke bridge

### 3. Possible intermediate resource-table helpers

I also checked the closest in-repo resource-table helpers:

- `fn_80191F64` in `src/hsd/hsd_wobj.c`
- `fn_80191ECC` in `src/hsd/hsd_wobj.c`

What this proves:

- the original game has at least one table/binder layer around resource data
- the repo contains partial visibility into that layer

What it does not prove:

- that the current host-safe repo can resolve a real `topmenu` / `title`
  content member into `HSD_PObjDesc`, `HSD_DObjDesc`, or a ready-to-render
  `HSD_PObj`

Why this is still a blocker:

- `fn_80191F64` remains asm-backed in the current repo build context
- the inspected code does not yet show an end-to-end path from raw FSYS member
  bytes to a renderable HSD descriptor tree

## What Real Repository-Backed Content Path Was Integrated

None in this step.

I did not make runtime or build changes because the blocker occurs before the
current native render bridge:

1. real repository content can be found and decompressed
2. the native bridge can already render a real `HSD_PObj` path
3. the repo does not yet expose the parser/resolver layer that joins those two
   facts together

That makes a code change here low-confidence and likely fake progress.

## Commands Run

Searches:

```powershell
rg -n "HSD_ArchiveParse|PObjLoadDesc|JObjLoadDesc|DObjLoadDesc|ArchiveParse|LoadDesc" src include asm docs
rg -n "topmenu\.fsys|title\.fsys|menu_title|menu_bg00|logo_gs|system_tool" src include asm docs orig
rg -n "struct HSD_Archive|typedef struct HSD_Archive|HSD_Archive" include src asm
```

Real-content inspection:

- parsed FSYS member tables from `orig/GC6E01/disc/files/*.fsys`
- decompressed representative members using the LZSS behavior documented in
  `src/game/fsys/fsys_decomp.c`

Regression checks rerun:

```powershell
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-pobj-smoke
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-scene-like-smoke
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-visible-smoke
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-scissor-retry
```

## Results

No new real-content-backed render path was integrated.

What was verified directly:

- real candidate content exists in-repo inside FSYS archives
- those members can be found and decompressed
- the current visible baselines still pass:
  - `--gsgfx-pobj-smoke`
  - `--gsgfx-scene-like-smoke`
  - `--gsgfx-visible-smoke`
  - `--gsgfx-scissor-retry`

What was not verified:

- feeding any real repository content blob into `fn_800DAD10`
- loading any real `HSD_PObjDesc` / `HSD_DObjDesc` from asset bytes
- a first real title/menu/game slice

## Exact Blocker Chain

1. Real content lives in FSYS members already present in the repository.
2. The current native bridge can render a narrow real `HSD_PObj` path once the
   object and display-list data already exist in C-accessible structures.
3. The repo does not currently expose a decompiled or host-safe parser/resolver
   that turns raw FSYS member bytes into `HSD_*Desc` graphs or ready-to-render
   HSD objects.
4. `HSD_PObjLoadDesc` and the adjacent HSD loaders are downstream of that
   missing step; they only consume resolved descriptors.
5. Therefore the current bridge cannot honestly replace bootstrap-authored
   object contents with real repository content yet.

## Is Engine Boot Smaller Now?

No, based on the current repo evidence.

This is an inference from the inspected code paths, not a verified runtime
experiment:

- engine boot would still need renderable object data from `title.fsys`,
  `topmenu.fsys`, or similar archives
- the current repo still lacks the parser/resolver layer that would materialize
  that data into HSD descriptors/objects
- so broadening into engine boot would not skip the current blocker; it would
  hit it from a larger path

## Next Smallest Milestone

The next smallest honest milestone is not broader GX work and not engine boot.

It is one of these, in order:

1. decompile or host-bridge the smallest real archive/content parser layer that
   can resolve one FSYS member into an `HSD_*Desc` graph
2. prove that one existing asm-backed resource-table helper is sufficient to
   expose such a descriptor without a full parser bring-up
3. only after that, feed one real repository-backed object into the existing
   `fn_800DAD10` path

Until that parser boundary is crossed, the visible result is still not backed by
real repository content.

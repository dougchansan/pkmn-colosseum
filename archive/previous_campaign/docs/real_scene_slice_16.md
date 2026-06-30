# Goal

Perform an evidence-led discovery pass to identify exactly one concrete next
reachable texture-expression family beyond the currently shared family cluster,
and determine the single smallest forced enabling surface.

## Outcome

Identified one concrete next reachable family:

- **one-stage RGBA8 no-TEV direct sample**

This family is already present in real scene/archive data, but the current shared
stage classifier rejects it because direct sample is still hard-coded to
`CMPR + no TEV` only.

## Why this candidate was chosen

I did not look for another abstract parser cleanup step.
I instead inspected real `scene_data` members outside the two already exercised
roots and searched for textured `TObj` chains that:

- successfully pass current archive / scene parsing
- have supported coord-source meanings
- do **not** require TLUT / CI acceptance
- do **not** require new TEV interpretation
- but still fail current shared stage / whole-expression classification

That makes this candidate the smallest next family because it avoids the three
larger blockers called out in slice 15:

- TEV signature widening
- `texCoordSrc` widening
- TLUT / CI acceptance

## Concrete candidate family

### Family

- one-stage **RGBA8 no-TEV direct sample**

### Exact scene/archive/sample

Representative sample:

- archive: `orig/GC6E01/disc/files/title.fsys`
- member: `logo_demo`
- public root: `scene_data`
- path:
  - `joint 0x26748`
  - `dobj 0x25F38`
  - `mobj 0x15758`
  - `pobj 0x25F20`
  - `tobj 0x156E8`
  - `image 0x156D0`
- observed texture facts:
  - format = `0x6` (`GX_TF_RGBA8`)
  - `tlutOffset = 0`
  - `tevOffset = 0`
  - `raw texCoordSrc = 4` -> resolved coord id `0`
  - size = `540 x 224`
  - flags = `0x300080`

A second matching sample of the same family appears at:

- `joint 0x26848 -> dobj 0x26098 -> mobj 0x155EC -> tobj 0x1557C`

So this is not a one-off anomaly; it is a repeated family inside the same real
scene member.

## Exact reason the current shared path rejects it

This sample already passes the earlier parser gates:

- it is in a real `scene_data` member
- the `TObj` and `ImageDesc` are valid
- `tlutOffset == 0`, so it is **not** blocked by CI/TLUT rejection
- `texCoordSrc = 4`, so it is **not** blocked by coord-source resolution
- `tevOffset == 0`, so it is **not** blocked by TEV signature interpretation

The rejection happens at **shared stage classification**:

- `ClassifyTextureExpStageKind(...)` currently recognizes direct sample only
  when:
  - texture format is `GX_TF_CMPR`
  - `tevArchiveOffset == 0`
  - `tev.kind == PCPORT_TRANSLATED_TEV_NONE`

The candidate sample satisfies the no-TEV part, but its format is
`GX_TF_RGBA8`, not `GX_TF_CMPR`, so the current classifier returns
`PCPORT_TEXP_STAGE_NONE`.

That means the current shared path cannot classify it into either:

- a supported stage kind
- or a supported whole-expression kind

## Single smallest forced enabling surface

**Forced surface first:**
- **other narrowly scoped shared surface**
- specifically: **shared stage classifier widening for no-TEV direct samples to
  admit non-CMPR decoded texture formats, starting with RGBA8**

## Why this is the smallest correct next step

It is smaller than the other candidate forcing surfaces because this sample does
**not** require them:

- not TEV signature widening (`tevOffset == 0`)
- not `texCoordSrc` widening (`rawSrc = 4` already resolves)
- not TLUT / CI acceptance (`tlutOffset == 0`)

The first forced blocker is therefore purely the direct-sample stage-family
predicate.

In other words, the next shared family is already reachable as soon as the
shared classifier stops treating “direct sample” as “CMPR-only.”

That is why this sample is a stronger next target than the other unsupported
`title.fsys:logo_demo` textures that still carry non-null TEV payloads.

## What remained narrow or family-specific

Unchanged after this discovery pass:

- shared stage kinds still only recognize:
  - direct sample (currently CMPR-only)
  - `I8` ramp sample
  - `I8` mask modulate
- shared whole-expression kinds still only recognize:
  - direct sample
  - `I8` ramp
  - `I8` ramp + mask
- TEV parsing still only recognizes the current narrow `I8` ramp signature
- `texCoordSrc` resolution is still limited to the currently verified meanings
- TLUT / CI-backed textures are still rejected during translation
- engine-owned title/menu/game boot and update-flow work remain out of scope

## Evidence collection method

I used an ad hoc read-only archive scan against real `scene_data` members in
menu-adjacent FSYS archives already present in the repository, including:

- `topmenu.fsys`
- `pda_menu.fsys`
- `title.fsys`
- `pokemonchange_menu.fsys`
- `colosseumbattle_menu.fsys`
- `carde_menu.fsys`
- `pcbox_menu.fsys`
- `name_entry_menu.fsys`

The scan extracted real FSYS members, decompressed LZSS members, relocated HSD
archives, walked `scene_data` joint/DObj/MObj/TObj` paths, and compared each
texture node against the current shared parser/classifier rules.

## Exact verification commands run

No code changes were required for slice 16, so no new implementation build was
strictly necessary.

For baseline safety and continuity, the current verified target was still run:

### Build

```powershell
cmd /c 'call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" && cmake --build build_pcbootstrap --config Debug --target pcport_bootstrap'
```

### Required smokes

```powershell
build_pcbootstrap\Debug\pcport_bootstrap.exe --real-textured-scene-slice-smoke
build_pcbootstrap\Debug\pcport_bootstrap.exe --real-scene-slice-3-smoke
build_pcbootstrap\Debug\pcport_bootstrap.exe --real-scene-slice-4-smoke
build_pcbootstrap\Debug\pcport_bootstrap.exe --real-tev-scene-slice-3-smoke
build_pcbootstrap\Debug\pcport_bootstrap.exe --real-tev-scene-slice-2-smoke
build_pcbootstrap\Debug\pcport_bootstrap.exe --gsgfx-visible-smoke
```

## Exact observed results

All baseline verification commands passed.

### Build

- `cmake --build build_pcbootstrap --config Debug --target pcport_bootstrap`
  - passed
  - observed: `ninja: no work to do.`

### Smokes

- `--real-textured-scene-slice-smoke`
  - passed
  - observed: `kind=3 stage0=3 stages=1 baked=1228800`
- `--real-scene-slice-3-smoke`
  - passed
- `--real-scene-slice-4-smoke`
  - passed
- `--real-tev-scene-slice-3-smoke`
  - passed
- `--real-tev-scene-slice-2-smoke`
  - passed
- `--gsgfx-visible-smoke`
  - passed

## Remaining blockers

1. The next family is now identified, but not yet admitted.
2. The direct-sample shared stage-family predicate is still narrower than the
   reachable data and remains CMPR-only.
3. After that family lands, other candidate families in `title.fsys:logo_demo`
   still appear to require additional TEV-surface work.
4. TEV, `texCoordSrc`, and TLUT/CI remain narrow outside the newly identified
   candidate.

## Why engine boot is still not the next step

Engine boot is still not the next step because the discovery pass found a
concrete next reachable family already living inside the shared real content
path.

The smallest next implementation step is now clear and local:

- broaden the shared no-TEV direct-sample stage family beyond CMPR-only,
  beginning with the observed RGBA8 no-TEV sample

That is still smaller, safer, and more directly verifiable than any engine
lifecycle work.

## Git / checkpoint note

The workspace remains dirty outside this slice, so slice 16 should not force a
commit.

This slice is a docs-first discovery result and should stop here.

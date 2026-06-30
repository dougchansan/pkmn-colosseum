# RE Workflow Playbook — Reversing Game Formats / Decompiling for the PC Port

A reusable methodology, derived from how the Genius-Sonority overworld **motion
format** was cracked (character animation: `scene_data` → motion bank → playing
idle/walk on Wes). Hand this to another agent/team verbatim.

## Core principle
**Trust the disassembly and the build symbol map. Distrust the annotations.**
The `// Proposed:` names in `symbols.txt` and the descriptive function names in
the `src/**/*.c` headers are AI-generated guesses and are frequently **wrong**.
Example that bit us: `fn_80161134` is annotated `peopleFieldMotionMain` but is
actually `_GetInputValue`. Every claim must be grounded in actual PowerPC
instructions or `config/GC6E01/symbols.build.txt`.

## Assets (know these paths)
- **Full disassembly:** `build/GC6E01/asm/auto_01_800055E0_text.s` — one file,
  all of main.dol `.text`. Blocks are `.fn NAME, global` … `.endfn`; each line is
  `address | file-offset | bytes | mnemonic`.
- **Reliable symbols:** `config/GC6E01/symbols.build.txt` (real names).
  `config/GC6E01/symbols.txt` adds unreliable `// Proposed:` guesses — ignore those.
- **Boundary:** main.dol `.text` ends at **0x80266358**. Addresses ≥ ~0x80300000
  live in **`common_rel`** (a separate relocatable module inside `common.fsys`),
  not in that asm file. Note it as a boundary; stay in main.dol where possible.
- **HSD struct headers:** `include/hsd/*.h`. **Working host ports** to compare
  against: `pcport/hsd_host.c`, `pcport/hsd_fobj_host.c`, `pcport/real_content_host.c`.

### Disassemble one function
```bash
awk '/\.fn fn_XXXXXXXX,/{p=1} p{print} p&&/\.endfn/{exit}' \
    build/GC6E01/asm/auto_01_800055E0_text.s
```
### Resolve a symbol's real name / address
```bash
grep -E "0xADDR|the_name" config/GC6E01/symbols.build.txt
```

## The loop (per target)
1. **Anchor on a real named function** near the feature — grep `symbols.build.txt`
   for domain words (`Motion`, `Anim`, `Model`, `HSD_`, `Setup`, `Load`, …).
   Real names are your entry points.
2. **Disassemble it**, follow register dataflow, recover the **struct offsets** it
   touches (`lwz r,0xNN(r)` = a field read; `stw` = a field write). Write the
   offsets down as a struct map.
3. **Follow the calls** — every `bl fn_YYYY` is the next node. Recurse until you
   reach a function you fully understand or a known-ported equivalent.
4. **Map data → behavior:** translate the offset chain into plain language —
   "starting from `scene_data` / the archive, field X at offset Y holds Z." That
   mapping *is* the deliverable.
5. **Cross-check against an existing port primitive.** Much "custom" format is
   actually stock HSD. Compare recovered layouts to `include/hsd/*.h` and the
   working `pcport/hsd_*_host.c`. **If it matches, reuse the existing code — don't
   rewrite.** (The motion data turned out to be stock HSD AnimJoint/AObjDesc/
   FObjDesc, so the existing FObj interpreter played it as-is.)

## Validate empirically — don't trust the trace alone
- Build a **tiny env-gated probe** in the host port that loads the real asset and
  prints the structure / steps the system, then run it on real data
  (`build_pc/pcport_bootstrap.exe`). The probe is how you *prove* the offset map.
  Useful signals: "field is an array of N pointers, each an AnimJoint",
  "stepping advances `curr_frame`", "per-frame checksum **varies** = real motion
  vs **constant** = a static pose."
- Capture **before/after artifacts** (BMP dumps → PIL diff) to confirm a visible
  effect and to catch the "it links but does nothing" failure mode.

## Parallelize the deep tracing (the force multiplier)
Spawn **focused subagents** (general-purpose / architect, read-only), one per
independent trace, in a single message so they run concurrently. Give each:
- exact paths (`build/GC6E01/asm/...`, `symbols.build.txt`) + the `awk` extract command,
- the **specific functions** to disassemble and the **specific question**
  ("recover the struct layout at offsets X/Y/Z", "does this match stock HSD AObjDesc?"),
- a **structured deliverable spec**: struct maps with hex offsets + quoted disasm
  lines (addr + mnemonic) backing every claim.

For the motion crack: Agent A traced the **loader** (archive → model resource →
motion table); Agent B decoded the **anim-attach format** (is it stock HSD?). They
cross-validated; the parent integrated and wrote the probe.

## Gotchas that bit us
- **Annotations lie** — verify names against `symbols.build.txt` / behavior.
- **Intra-TU calls bind locally** — a host override of `fn_X` does **not** intercept
  a same-file caller of `fn_X`. (This silently disabled the animation drive: the
  inert `HSD_JObjAnim` in `hsd_jobj.c` was called intra-TU by `HSD_JObjAnimAll`, so
  the host override never fired.) Override the *caller* if needed.
- **A "constant / frozen" result** often means you're reading a **pointer to an
  array** as if it were the struct. (The motion bug: `Resource+0x4` is `AnimJoint**`
  — an array of motions — but was attached as a single `AnimJoint*` → garbage pose.
  Fix: deref `array[idx]`.) Always re-check `T*` vs `T**`.
- **Silent no-op build:** if `pcport_main.c` fails to compile, `main()` gets
  auto-stubbed and the exe runs but produces nothing. Always:
  `python tools/pcport_link.py 2>&1 | grep "failed to compile"`.
- **`common_rel` addresses** (≥ ~0x80300000) are a different module, not in the
  main asm file.

## Build / run reference
- **Build (authoritative exe):** `python tools/pcport_link.py` (clang). Rebuild the
  shim lib via `tools/pcport_build.ps1` only if `gx_*` / audio shims changed.
- **Headless render + dump:**
  `PCPORT_DEBUG_NEWGAME=1 PCPORT_MENU_FRAMES=N PCPORT_DUMP=out.bmp build_pc/pcport_bootstrap.exe --menu`
- **Enumerate an archive:** `PCPORT_FSYS_LIST=<path.fsys> build_pc/pcport_bootstrap.exe`
- **Inspect an HSD member's layout:**
  `PCPORT_HSD_SWIZ_TEST=1 PCPORT_SWIZ_ARCHIVE=<fsys> PCPORT_SWIZ_MEMBER=<name> build_pc/pcport_bootstrap.exe`

## Worked example — the motion crack (summary)
1. Anchored on `menuModelSetMotion` (0x80109894, a real name) → it calls
   `fn_800ECCA8(model+0x24, motionId)`.
2. `fn_800ECCA8` reads `model+0x4` (a "resource"), `model+0x84` (motion count),
   indexes `resource+0x4[motionId]` and `resource+0xC[motionId]`, then calls
   `fn_801A2B5C(jobjRoot, animSet, 0, matAnim)`.
3. Followed the loader (`fn_801013A0` spawner → `fn_800E51A4` ctor): the archive's
   `scene_data` → `Resource[]`; `Resource[0]+0x0` = JObjDesc (skeleton),
   **`Resource[0]+0x4` = pointer to a NULL-terminated array of `HSD_AnimJoint*`
   = the motion bank**, indexed by motion id.
4. Decoded the attach (`fn_801A2B5C` → `fn_801C2670` → `fn_80199794`): the data is
   **stock HSD** (AnimJoint / AObjDesc / FObjDesc / keyframe encoding all match) →
   the existing `pcport/hsd_fobj_host.c` interpreter plays it unchanged.
5. Proved it with a probe: deref `array[idx]`, attach, step — ken_b1 has **11
   motions**, idx 0 = T-pose bind, idx 1–10 = real idle/walk/run (29 animated
   joints, time-varying SRT, cyclic walk). Wired idle-on-spawn + walk-when-moving.

**One-line essence:** anchor on a real symbol → disassemble → recover struct
offsets → follow `bl` calls → map archive-bytes-to-behavior → prove it with an
env-gated probe on real data → reuse stock-HSD code where the format matches. Fan
the disassembly out to parallel subagents with precise, structured asks.

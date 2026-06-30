# Decomp notes: src/game/gs_event_exec.c

## Status snapshot
17/30 @ 100% (56.7%, as of 2026-05-13)

⚠️ **Hallucination warning:** Agent w8 (2026-05-13) claimed fn_80014D1C
reached 100% via `entry->b - delta * 0x1F` but post-cherry-pick master
measures **99.68%**. Either the agent's worktree measurement was wrong, or
the cherry-pick lost a non-obvious change. Re-verify carefully before
trusting any claimed 100% in this file.

## Recently landed (2026-05-13, agent w8)

Three commits cherry-picked; reported gains were partial improvements (not
all 100% as claimed):

- **fn_800138B4** — 92% → 98.98%. Deferred `idx = -1; i = 0` after if/else calls.
- **fn_80013DFC** — 92% → 98.29%. Same deferred init + removed `(u16)` cast on `fn_801440A0`.
- **fn_80013F80** — 90% → 96.88%. Deferred init + condition inversion `!= 0` → `== 0`.
- **fn_80014E50** — 90% → 96.69%. `*(s8*)(ctx+0x95) = new_slot` instead of array cast.
- **fn_8001374C** — 88.93% → 95.60%. Same deferred init pattern + moved `*out = 0` into both branches.
- **fn_80014D1C** — **claimed 100%, measured 99.68% on master**. Re-investigate.

Technique pattern: **deferred initialization after if/else calls** repeatedly worked here — CW's register allocator handles the post-call init differently than a pre-call init.

## Blocked near-misses

- **fn_800138B4 / fn_80013DFC / fn_8001374C** @ ~95-98% — `frame-size-quirk`
  - Symptom: target's frame is +0x10 from ours, no source fix found.
  - Referenced in `feedback_gs_event_exec_pipeline.md` ("stack-alignment quirk in CW 1.3").
  - Tried: deferred init (helped, but ceiling still ~96-98%), `f32_array_frame_size` trick.
  - Next leads: try `u8 _pad[0x10]` at end of frame.
  - Last attempt: 2026-05-13 (w8)

- **fn_80014D1C** @ 99.68% — **re-investigate hallucinated 100% claim**
  - Agent w8 claimed `entry->b - delta * 0x1F` fixed it but master disagrees.
  - Possible avenue: check git diff between w8's worktree and master post-cherry-pick to see if a hunk was lost.
  - Last attempt: 2026-05-13 (w8, claim unverified)

## Untouched near-misses
- fn_80012D20, fn_80014234, fn_80014C38, fn_80014E50 (some now landed-partial above)

## Session log

- **2026-05-13 (w8)** — 3 commits cherry-picked (`6ee0096`, `68f52e6`, `dcf97e9`). Master gained partial improvements (none new at 100%). Hallucinated 100% on fn_80014D1C documented above.
- **2026-05-13 (x10, wave 2)** — fn_80014AE4 hit 100% (removed `(u8)(s32)` cast). fn_80014D1C originally 94.9% → 99.7% (claimed at that time; current master 99.68%).

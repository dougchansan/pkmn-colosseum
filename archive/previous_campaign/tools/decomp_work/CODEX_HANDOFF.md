# Codex Handoff Prompt — Pokémon Colosseum Decompilation

Copy this into Codex to continue the decomp work.

---

## Prompt for Codex

You are continuing decompilation work on Pokémon Colosseum (GameCube, GPXE01). The goal is to replace assembly wrappers (`#if 1 / asm void`) with matching C89 code that compiles and matches the original binary byte-for-byte.

### Current State
- 23 functions matched to 100% this session (manual Claude work)
- 1,002 asm wrappers remain across 4 complexity tiers
- Full orchestration system built: `tools/decomp_scheduler.py`, `tools/decomp_agent.py`, `tools/decomp_benchmark.py`
- 7 models benchmarked — Kimi K2.5 (1.9s/fn) and Codestral:22b (5.4s/fn) are the best

### Your Task

1. **Read `docs/key_techniques.md`** — this contains ALL the CW compiler matching rules
2. **Read `tools/decomp_work/few_shot_examples.md`** — 9 proven match patterns
3. **Read `tools/decomp_work/AGENTS.md`** — coordination protocol
4. **Run `python tools/decomp_scheduler.py --scan`** to see current queue
5. **Pick simple/medium tier functions** and decompile them

### Workflow for Each Function

```bash
# 1. Find a function to work on
python tools/decomp_scheduler.py --scan
# Look at tools/decomp_work/work_queue.json for targets

# 2. Read the assembly
cat src/path/to/file_fn_XXXXXXXX.inc

# 3. Read context in the C file (find the #if 1 block)
grep -n "fn_XXXXXXXX" src/path/to/file.c

# 4. Write the C replacement (see rules below)

# 5. Compile
python tools/compile_check.py src/path/to/file.c

# 6. Match test
python tools/match_test.py fn_XXXXXXXX

# 7. If < 100%, get diff and iterate
# 8. If 100%, commit
```

### Critical Rules (DO NOT VIOLATE)

1. **C89 only** — ALL declarations before statements in each block
2. **NEVER use float literals** for sdata2 returns — use `extern f32 lbl_XXXXXXXX`
3. **Compute SDA addresses**: `SDA_BASE = 0x80480820` (r13), `SDA2_BASE = 0x804836A0` (r2)
   - Signed 16-bit: if raw >= 0x8000, subtract 0x10000
   - Look up label: `grep "ADDRESS" config/GC6E01/symbols.txt`
4. **Block scoping** `{ }` — use when asm loads same r13 global twice
5. **Leaf functions** (nofralloc, no `bl`) — do NOT add pragmas, use default O4
6. **Thunks** (stwu/mflr/bl/epilogue) — just `extern void target(); void fn() { target(); }`
7. **Call-return-0 with r3 save/reload** — needs `#pragma optimization_level 0`
8. **Check compiler version** — Dolphin SDK files may need `GC/1.2.5n` in `config/GC6E01/compile_config.json`
9. **`subf rD, rA, rB`** = rB - rA (reversed operands!)
10. **`clrrwi rD, rS, N`** = rS & ~((1<<N)-1)
11. **ALWAYS revert on failure** — never leave a file in a broken state
12. **ALWAYS verify compile before claiming match**

### Pattern Quick Reference

| ASM Pattern | C Pattern |
|-------------|-----------|
| `lwz r3, sym(r13); lwz r3, OFF(r3); blr` | `extern u32 sym; return *(u32*)((u8*)sym + OFF);` |
| `lwz r4, sym(r13); stw r3, OFF(r4); blr` | `extern u32 sym; *(u32*)((u8*)sym + OFF) = val;` |
| `lfs f1, OFF(r2); blr` | `extern f32 lbl_XXX; return lbl_XXX;` |
| `stwu/mflr/bl target/epilogue` | `extern void target(); void fn() { target(); }` |
| `cmplwi r3, 0; beq @null; lfs f1, OFF(r3); blr` | `if (ptr != NULL) return *(f32*)(ptr+OFF); return lbl_XXX;` |
| double `lwz` of same r13 sym | Use `{ }` block scoping to prevent CSE |
| `stw r3, 0x8(r1); lwz r3, 0x8(r1); bl target; li r3, 0` | `#pragma optimization_level 0` call + return 0 |

### Easy Wins to Start With

These are the simplest remaining functions (sorted by asm line count):

```
fn_8009A0C0   1 line   src/dolphin/os/OSException.c     (bla instruction)
fn_8019C6FC   3 lines  src/hsd/hsd_initialize.c         (compare global to 3)
fn_800F02F4   5 lines  src/game/gs_texture.c            (stack manipulation)
fn_80162070   7 lines  src/game/people/people_field.c    (PRNG)
fn_80163490   7 lines  src/game/people/people_field.c    (spin-wait)
```

### Commit Format

```
Phase 3: decompile N functions in <file> (all 100% match)

- fn_XXXXXXXX: brief description
- fn_YYYYYYYY: brief description

Co-Authored-By: Codex (o4-mini) <noreply@openai.com>
```

### Infrastructure Available

| Backend | Command | Use For |
|---------|---------|---------|
| Kimi K2.5 | API at `api.moonshot.ai` (key in opencode.json) | Fast drafts (1.9s/fn) |
| Codestral:22b | Ollama at 10.0.0.3:11434 | Best local model (5.4s/fn) |
| Ollama local | localhost:11434 | qwen3:8b, qwen2.5:14b on 9070 XT |
| This tool | `python tools/decomp_agent.py --backend ollama --function fn_XXX` | Full pipeline |

### Files to Read First

1. `docs/key_techniques.md` — ALL matching rules
2. `tools/decomp_work/few_shot_examples.md` — proven patterns
3. `tools/decomp_work/AGENTS.md` — coordination protocol
4. `docs/multi_agent_decomp_plan.md` — full system architecture

# Multi-Agent Decompilation Plan

**Project:** Pokémon Colosseum (GPXE01) GameCube Decompilation
**Date:** 2026-04-09
**Goal:** Decompile all 1,022 remaining asm wrappers to 100% byte-match

> Update (2026-04-10, `codex-rescue-d`): verified 11 additional wrapper
> removals in this branch-local overnight lane:
> `fn_800301A8`, `fn_800301AC`, `fn_80030370`, `fn_8003037C`,
> `fn_80030428`, `fn_80069048`, `fn_80176684`, `fn_8017669C`,
> `fn_80177A38`, `fn_8018F470`, `fn_801A6DDC`.
> Branch-local scheduler state after rescan: 1,038 remaining wrappers and 26
> simple-tier wrappers. Current productive pattern is targeted manual rescue in
> isolated worktrees; bulk Ollama generation is still high-noise/compile-error.

---

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────┐
│                    TASK SCHEDULER                           │
│              decomp_scheduler.py --scan                     │
│         Classifies 1,022 wrappers into 4 tiers             │
└────────────────────────┬────────────────────────────────────┘
                         │
            ┌────────────┼────────────┐
            ▼            ▼            ▼
   ┌──────────────┐ ┌──────────┐ ┌──────────────┐
   │   FREE GPU   │ │  CODEX   │ │  CLAUDE CODE │
   │  RTX 3090    │ │  o4-mini │ │  Opus 4.6    │
   │  10.0.0.3    │ │  (free)  │ │  (paid)      │
   │              │ │          │ │              │
   │ deepseek-    │ │ via CLI  │ │ Manual +     │
   │ coder-v2:16b│ │ full-auto│ │ Agent teams  │
   │              │ │          │ │              │
   │ simple +     │ │ simple + │ │ complex +    │
   │ medium tier  │ │ medium + │ │ hard tier    │
   │              │ │ complex  │ │              │
   └──────┬───────┘ └────┬─────┘ └──────┬───────┘
          └───────────────┼──────────────┘
                          ▼
             ┌────────────────────────┐
             │    VERIFICATION        │
             │  compile_check.py      │
             │  match_test.py         │
             │  objdiff instruction   │
             │  comparison            │
             └────────────┬───────────┘
                          │
              ┌───────────┼───────────┐
              ▼           ▼           ▼
         ┌────────┐ ┌─────────┐ ┌─────────┐
         │ 100%   │ │ < 100%  │ │ COMPILE │
         │ MATCH  │ │ PARTIAL │ │ ERROR   │
         │        │ │         │ │         │
         │ Commit │ │ Retry   │ │ Revert  │
         │ + Push │ │ w/ diff │ │ + Log   │
         └────────┘ │ feedback│ └─────────┘
                    └─────────┘
```

---

## Tier Classification

| Tier | ASM Lines | Count | Primary Backend | Fallback |
|------|-----------|-------|----------------|----------|
| **Simple** | 2-10 | 21 | Ollama (deepseek-coder-v2:16b) | Codex (o4-mini) |
| **Medium** | 11-30 | 116 | Codex (o4-mini) | Claude Code |
| **Complex** | 31-80 | 287 | Claude Code (manual) | Claude + retry |
| **Hard** | 80+ | 598 | Claude Code (Opus) | Team agents |

---

## Backend Details

### 1. Ollama on RTX 3090 (FREE)
- **Host:** douglaswhittingham@10.0.0.3
- **GPU:** NVIDIA RTX 3090 (24GB VRAM)
- **Storage:** /storage SSD (262GB free)
- **Model:** deepseek-coder-v2:16b (8.9GB, best benchmark score)
- **Speed:** ~14s per function
- **Strengths:** Understands PPC→C patterns, correct loop/branch logic
- **Weaknesses:** Uses stdint.h types instead of project types, sometimes adds unnecessary casts
- **Command:** `python tools/decomp_agent.py --backend ollama --tier simple --count 10`

### 2. Codex CLI (FREE with OpenAI account)
- **Version:** codex-cli 0.118.0 + oh-my-codex 0.11.12
- **Model:** o4-mini (full-auto approval mode)
- **Strengths:** Strong reasoning, understands compiler behavior
- **Weaknesses:** Slower, may try to modify files directly
- **Command:** `python tools/decomp_agent.py --backend codex --tier medium --count 5`

### 3. Claude Code (PAID — use strategically)
- **Model:** Opus 4.6 (1M context)
- **Strengths:** Best at complex register allocation, CW codegen quirks, pragma tuning
- **Best for:** Complex/hard tier, near-miss debugging, technique discovery
- **Workflow:** Manual decompilation with compile+match feedback loop

### 4. OpenCode + oh-my-openagent (FREE cloud models)
- **Version:** OpenCode 1.3.0, oh-my-openagent 3.16.0
- **Models:** Gemini free tier, other cloud models
- **Status:** Available but not yet configured
- **Command:** `python tools/decomp_agent.py --backend opencode --tier simple`

---

## Anti-Regression System

### Lock Protocol
```
tools/decomp_work/locks/<fn_name>.lock
```
- Created before work starts on a function
- Contains PID, timestamp, host
- Auto-expires after 30 minutes (stale lock cleanup)
- Only ONE agent can work on a function at a time
- Only ONE agent can modify a .c file at a time

### Backup + Revert
1. Before applying any change, save the original .c file to `results/<fn>_backup.c`
2. Apply the generated C code
3. Compile check — if fails, **revert immediately**
4. Match test — if < 100%, **revert immediately**
5. Only commit on 100% match

### Progress Tracking
```
tools/decomp_work/progress.json    — completed/failed function lists
tools/decomp_work/work_queue.json  — all remaining wrappers with metadata
tools/decomp_scheduler.py --status — human-readable progress report
```

### Commit Protocol
- Batch commits every 5-10 matched functions
- Format: `Phase 3: decompile N functions in <file> (all 100% match)`
- Include function names and brief descriptions
- Push to GitHub after each batch
- GitHub Actions CI verifies compilation on push

---

## Prompt Engineering

### Pattern Detection
The scheduler auto-detects asm patterns and selects the best few-shot example:

| Pattern | Detection | Example Provided |
|---------|-----------|-----------------|
| thunk | stwu + single bl | Pass-through call |
| getter | lwz from r13 + lwz field + blr | Global pointer getter |
| setter | lwz from r13 + stw field + blr | Global pointer setter |
| float_return | lfs from r2 + blr | sdata2 float return |
| conditional_float | cmplwi + beq + lfs | Null-check float getter |
| output_params | stw to 0(r3) + stw to 0(r4) | Two output params |
| double_load | same r13 sym loaded twice | Block-scoped reloads |
| aligned_math | clrrwi present | Round-up/alignment |

### Retry with Diff Feedback
On failure, the objdiff output is fed back to the model:
```
PREVIOUS ATTEMPT FAILED. Here is the instruction diff:
=== TARGET ===
  0    lwz r4, 0x0(r3)
  1    lwz r0, 0x4(r3)
=== YOURS ===
  0 !! lwz r0, 0x0(r3)
  1 !! stw r0, lbl@sda21

Fix the mismatches.
```

---

## Benchmark Results

| Rank | Model | Structural Score | Speed | Notes |
|------|-------|-----------------|-------|-------|
| 1 | deepseek-coder-v2:16b | 100% (13/13) | 14.3s/fn | **Default for Ollama** |
| 2 | qwen2.5-coder:7b | 100% (13/13) | 1.3s/fn | Fast but copies asm literally |
| 3 | qwen2.5-coder:32b | 76.9% (10/13) | 40.5s/fn | Timeouts, not recommended |

### Fine-Tuning Path
- Training data: 13 examples exported in ChatML + Alpaca format
- Location: `/storage/finetune/` on GPU machine
- Base model: qwen2.5-coder:7b (fastest, LoRA fine-tune for accuracy)
- Tool: LLMFit (already installed on 3090 machine)

---

## Execution Plan

### Phase 1: Simple Tier Sweep (21 functions)
1. Generate all 21 prompts: `python tools/decomp_scheduler.py --generate 21 --tier simple`
2. Run Ollama batch: `python tools/decomp_agent.py --backend ollama --tier simple --count 21`
3. Fix apply_result bugs for remaining failures
4. Manual fixup for any that don't match
5. **Expected yield:** 10-15 matches (50-70% automated)

### Phase 2: Medium Tier Batch (116 functions)
1. Run Codex on medium tier: `--backend codex --tier medium --count 20`
2. Run Ollama on medium tier in parallel
3. Claude handles any that both fail on
4. **Expected yield:** 40-60 matches

### Phase 3: Complex Tier (287 functions)
1. Claude Code manual decompilation (highest value per token)
2. Codex as first-pass generator, Claude for refinement
3. Focus on file-level batches (all functions in one .c file)
4. **Expected yield:** Steady progress, 5-10 per session

### Phase 4: Hard Tier (598 functions)
1. Claude Code with Opus for deep analysis
2. Agent teams for parallel research
3. Ghidra-assisted decompilation for the most complex functions
4. **Expected yield:** Long-term, project-level effort

---

## Commands Reference

```bash
# Scan and classify all remaining wrappers
python tools/decomp_scheduler.py --scan

# Generate prompts for a tier
python tools/decomp_scheduler.py --generate 10 --tier simple

# Run automated decomp via Ollama (free GPU)
python tools/decomp_agent.py --backend ollama --tier simple --count 5

# Run via Codex (free)
python tools/decomp_agent.py --backend codex --tier medium --count 5

# Check progress
python tools/decomp_scheduler.py --status

# Verify a specific function
python tools/decomp_scheduler.py --verify fn_XXXXXXXX

# Run model benchmark
python tools/decomp_benchmark.py --run "deepseek-coder-v2:16b"

# Show leaderboard
python tools/decomp_benchmark.py --leaderboard

# Export fine-tuning dataset
python tools/decomp_benchmark.py --export-finetune
```

---

## Key Documentation

| Document | Purpose |
|----------|---------|
| `docs/key_techniques.md` | CW codegen matching guide |
| `docs/matching_guide.md` | Original matching reference |
| `docs/acgc_pc_port_analysis.md` | PC port lessons from Animal Crossing |
| `docs/pc_port_design.md` | GX→OpenGL translation design |
| `docs/phase3_plan.md` | Phase 3 work packets |
| `tools/decomp_work/AGENTS.md` | Agent coordination protocol |
| `tools/decomp_work/few_shot_examples.md` | 9 proven match patterns |

---

## Infrastructure Checklist

- [x] decomp_scheduler.py — scan, classify, generate prompts
- [x] decomp_agent.py — multi-backend runner with retry
- [x] decomp_benchmark.py — model scoring and leaderboard
- [x] RTX 3090 Ollama setup with deepseek-coder-v2:16b
- [x] Codex CLI integration
- [x] Lock-based anti-regression
- [x] Backup + auto-revert on failure
- [x] Few-shot pattern examples
- [x] GitHub Actions CI scaffold
- [x] Fine-tuning dataset exported
- [ ] Fix apply_result for all wrapper patterns
- [ ] OpenCode free model configuration
- [ ] decomp.me integration
- [ ] Progress badges in README
- [ ] LoRA fine-tune qwen2.5-coder:7b
- [ ] Codex backend real-world testing

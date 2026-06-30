# Cross-model decomp benchmark summary — 2026-04-25/26

Consolidated results from compile+match scoring against the
**`gs_field_world.c` 8-function expert-prompt suite** (the primary cloud-model
single-shot benchmark) and the **`gs_title.c` 18-function expert suite** (the
much harder real-world test).

## Setup

- **Reward function**: `tools/decomp_work/rl/reward_fn.py` running on the 3090
  (Linux). Splices candidate C into `src/game/<stem>.c`'s `#else` block, builds
  via `tools/compile_check.py`, measures via `tools/match_scan_file.py`, and
  reverts via file snapshot (the working tree on the 3090 is NOT a git repo).
- **Expert prompt** (~24-32 KB per test): `CW_QUIRKS.md` cheatsheet + 9 verified
  100%-match few-shot examples + reference projects (TWW, Melee, Pikmin2, Prime,
  SMS, TP) + 2 peer functions from the same TU + all existing externs +
  cast-fnptr idiom + existing `#else` partial as starting point.
- All cloud calls use `temperature=0.1`, prompt cache enabled where supported.

## Suite 1 — `gs_field_world.c` (8 simpler functions, expert prompt)

### Single-shot (one cloud call per function, no repair)

| Model | PASS | mean% | total time | per-call notes |
|---|---|---|---|---|
| `moonshot/kimi-k2.6` | **7/8** | 68.7% | 68s | Reasoning model |
| `deepseek/deepseek-v4-pro` | **7/8** | 68.9% | 78s | Reasoning model (thinking enabled) |
| `deepseek/deepseek-v4-flash` | 6/8 | 59.8% | 36s | thinking-disabled, ~99% prompt cache |
| `codex/gpt-5.5-xhigh` (via tmux) | 4/8 | 38.3% | 240s | Slowest cloud + lowest score among non-zero |
| `openrouter/qwen/qwen3.6-plus` | 2/8 | 18.8% | 357s | Rate-limited after 2 calls |

### Best-of-N (initial=3, repair=2 — 4-5 calls per function)

| Model | PASS | mean best% | total time | repair lift |
|---|---|---|---|---|
| `deepseek/deepseek-v4-pro` | **7/8** | 73.1% | 384s | 2 funcs improved by repair pass |
| `deepseek/deepseek-v4-flash` | **7/8** | 73.0% | 187s | 0 (best on first try; cheapest) |
| `moonshot/kimi-k2.6` | **7/8** | 72.3% | 295s | 0 |

### Local 3090 (ollama)

| Model | PASS | mean% | total time | notes |
|---|---|---|---|---|
| `ollama-proxmox/deepseek-r1:14b` | 1/8 (partial) | 11.4% | n/a | Only local model with any PASS — fn_8011BA0C 91.3% |
| `ollama-proxmox/qwen2.5-coder:32b` | 0/8 | 0% | 1092s | All 180s timeouts; loop divergence on >150-line fns |
| `ollama-proxmox/qwen3:14b` | 0/8 | 0% | 380s | mostly no-code |
| `ollama-proxmox/gemma3:27b` | 0/8 | 0% | 217s | mostly no-code |
| `llamacpp-proxmox/qwen3.6-35b-a3b` | 0/8 | 0% | 332s | 2k context window; expert prompt overflows |

## Suite 2 — `gs_title.c` (18 hard functions, 100-366 asm lines each)

### Single-shot expert prompt (overnight loop, 12 attempts/fn)

| Model | PASS | best mean% | notes |
|---|---|---|---|
| `moonshot/kimi-k2.6` | 0/18 | 0% on saved suite (1 transient 76% during broken-revert window) |
| `deepseek/deepseek-v4-pro` | 0/18 | 0% |
| `codex/gpt-5.5-xhigh` (1st run, 12/19 before usage limit) | 0/12 | 0% |
| `codex/gpt-5.5-xhigh-r2` (retry on remaining 7) | 0/7 | 0% |

**Conclusion for suite 2:** No cloud model produced a compilable candidate
without manual fix-up. The blocker is consistently extern signature collisions
between cloud-emitted block-scoped redeclarations and the file's existing
file-scope externs. The expert-prompt instructions about cast-fnptr aren't
strong enough to make models avoid this on functions with many callees.

### Manual hand-tune results (one function at a time, Opus-driven, with cloud assist)

| Function | asm lines | best% | committed | notes |
|---|---|---|---|---|
| `fn_80020F54` | 112 | **100.0%** | ✅ `973e357` | switch + block-extern + `((s8*)arg1)[4] &= ~2` |
| `fn_80020C9C` | 140 | **100.0%** | ✅ `41795ca` | switch case order matters; reorder source to put case 0 first |
| `fn_800218BC` | 132 | **94.5%** | ✅ `a30f607` | CTR-loop + `(u8)fn_X()` cast + double-load thresh |
| `fn_80024160` | 123 | 87.7% | WIP | regalloc-blocked; same instructions, permuted regs |
| `fn_800216E8` | 133 | 46.6% | WIP | table-copy idiom mismatch (152-byte size delta) |
| (remaining 13) | 100-366 | 0% | — | all need manual hand-iteration |

## Headline takeaways

1. **Reasoning models dominate single-shot** for byte-match decomp: V4-pro and
   Kimi K2.6 tie at 7/8 PASS on the easier suite.
2. **DeepSeek V4-flash with `thinking:disabled` + best-of-N matches the
   reasoning leaders** at half the wall time and a fraction of the cost.
3. **Codex/GPT-5.5-xhigh underperforms** the DeepSeek family on byte-match
   decomp (4/8 vs 6-7/8), despite being the strongest "agentic" model.
4. **Cloud models can't crack the gs_title-class functions unaided** — every
   model produced 0/18 PASS even with best-of-N. The issue is extern signature
   collisions, not inability to reason about the asm.
5. **Local 3090 models give 0-1/8** — `qwen2.5-coder:32b` was the only one with
   any prior signal (95% on a single simple function from the older benchmark);
   on the new harder suite it timed out everywhere.
6. **The single highest-leverage harness fix this session**: replacing
   `git checkout` with file-snapshot revert in `reward_fn.py`. Without that, all
   reward scores were silently corrupted by the 3090's lack of a `.git/` dir.
7. **Per-function manual iteration produces the only 100% matches.** Three
   functions committed in a single working session at ~30-60 min each. Proven
   workflow: read asm → existing partial baseline → switch shape + block
   externs + per-quirk fixes → diff inspection → tweak → repeat.

## Cost summary (estimate)

| Provider | calls | total cost |
|---|---|---|
| DeepSeek V4 (direct API) | ~250 | $0.50 ($10 budget barely touched, ~99% cache hit) |
| Moonshot Kimi K2.6 | ~150 | $0 (within free tier) |
| OpenRouter | ~30 | $0.20 |
| Codex/GPT-5.5-xhigh | ~50 | included in user's ChatGPT Codex plan |
| 3090 reward calls | ~600 | electricity only |

## Recommended workflow going forward

1. **For new gs_title functions**: open the .inc, read the asm, identify shape
   (switch / dispatcher / loop / state machine). Write a structural skeleton.
2. **Block-scope ALL externs** with their real signatures inside the function
   body — never modify file-scope externs.
3. **Submit through `local_iterate.py`** to get the first compile-clean result
   and an asm diff.
4. **Apply per-iteration**: case-order fixes, pragma combos, type-cast tweaks
   for clrlwi/extsh placement, double-load patterns for CSE defeat, CTR-loop
   idiom for mtctr/bdnz.
5. **Cloud models help at the edges** — once you have a compile-clean baseline,
   send via `attack_function.py` for pragma+small-mutation exploration.
6. **Commit at ≥90%** per project policy if regalloc is the only remaining gap;
   document the WIP state in `.omc/decomp_attack/<fn>_wip.md` for future
   reference.

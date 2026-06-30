# SFT training results — qwen2.5-coder-7b on 70k CW decomp pairs

## Training run (2026-04-18 → 04-19)

- **Base model**: Qwen/Qwen2.5-Coder-7B-Instruct
- **Method**: QLoRA (4-bit base, LoRA r=32, 80.7M trainable / 7.7B total)
- **Dataset**: 70,585 matched C function bodies from 10 GameCube/Wii decomps
- **Hyperparams**: 1 epoch, lr 1e-5, batch 1, grad_accum 16, bf16, cosine LR
- **Hardware**: RTX 3090 (24GB), 4-bit quant
- **Duration**: 8h 57min (32,240 seconds)
- **Loss trajectory**: 2.5 → 0.743 (healthy convergence, no plateau)
- **Artifacts**: `/storage/finetune/sft/qwen7b-cw/final/` (LoRA adapter ~280MB)

## Eval results

### Test 1: inference with CW_QUIRKS-markdown prompt (our build_prompt.py output)
Result: **FAIL.** Model emitted a meta-description ("This is a detailed guide
for converting assembly code...") instead of code. The big-markdown prompt
is out-of-distribution relative to training.

### Test 2: inference with training-format prompt (Project/File/Library/Flags)
Result: **Partial success.** Model produced real C:
```c
int fn_80115C48(void) {
    int i;
    u32* ptr = lbl_80478FBC;
    for (i = 0; i < *ptr; i++) {
        if (*(u32*)(ptr + 1 + i * 0x4C) == gCurrentMapId) {
            return *(u32*)(ptr + 2 + i * 0x4C);
        }
    }
    return -1;
}
```

- ✅ Correct function name, real lbl_XXXX symbols, CW-style naming
- ✅ Structurally a table-walk (matches the actual function's shape)
- ❌ Invented `gCurrentMapId` (real signature takes `u32 key` arg)
- ❌ Array math wrong: `ptr + 1 + i * 0x4C` scales by sizeof(u32), should use `(u8*)ptr`
- ❌ Didn't use lbl_80478FB8 (count source), read count from ptr[0]
- ❌ Degenerate repetition in tail (classic fine-tune artifact)

Would NOT compile against real externs (`gCurrentMapId` undeclared). Match
score if forced-compiled: likely <40%.

## Interpretation

**The model LEARNED CW-style C patterns.** The style, naming, symbol usage,
and function structure are right. But semantics aren't anchored because the
training prompt format doesn't include the actual target asm — only a brief
natural-language description.

**Gap identified**: we trained on `(Project, File, Library, Flags) → C_body`
with no asm input. At inference we want `asm → C`. The model is doing
something useful but not what we ultimately need.

## Next steps

### 1. Retrain with asm input (proper asm → C)

Need to extract per-function asm for each matched C body. The decomp repos'
asm/ dirs contain only unmatched stuff, not per-function dumps for matched
functions. Options:

- **Build each project** → produce .o files → objdump → per-function asm.
  Expensive: needs per-project toolchain (different CW versions, flag sets).
- **Use Ghidra**: each project has a dol target; run Ghidra headless to dump
  per-function asm, pair with matched C by symbol name. More uniform.
- **Simpler**: since objdiff-cli works cross-project, dump asm from built .o's.
  Pikmin2 builds with `python3 configure.py && ninja`. Run on 3090, extract
  per-function asm from `build/**/*.o` via objdump.

### 2. Current SFT adapter is still useful as a "CW-style prior"

Even without asm input, the adapter teaches the base model CW idioms. Could
be combined with a rewriting/constraint pipeline:
1. Use adapter to draft CW-style C from minimal context
2. Use a separate pass (regex or compile-feedback) to fix specific errors:
   symbol names, arg types, array offsets
3. Iterate compile→match until score improves

### 3. Alternative: fine-tune on the pkmn-colosseum-specific mapping

We have 160 (asm, C) pairs in Colosseum with perfect symbol context. Training
a small LoRA *on top of this adapter* specifically on our 160 pairs might
transfer the symbol/offset knowledge we need. 160 is tiny but as a refinement
step after 70k warmup it could work.

### 4. RL with compile-match reward

This is where the real gain is. With the adapter as starting policy:
- Sample candidate C
- Run `reward_fn.compute_reward` (compile + match%)
- PPO/GRPO update
- Model converges to byte-matching C for functions similar to training set

Our existing `reward_fn.py` + `train_grpo.py` scaffolding handles this — just
point at the SFT adapter as the base.

## Realistic outcome assessment

**SFT alone is not sufficient.** The 70k training was useful groundwork —
the model knows CW-style C now. But for byte-matching, we need either
(asm → C) training pairs OR RL with compile feedback.

**Recommended**: skip retraining with asm. Go directly to RL on the existing
adapter. RL with `compile + match_scan` reward will teach the model what
byte-matching means specifically for our Colosseum target. Expect 2-3 days
of RL training for meaningful convergence.

## Cost paid so far

- Scaffolding: ~50k Opus tokens (this session)
- SFT training: 9h on 3090 (free)
- Model artifacts: ~280MB LoRA adapter
- Total cloud cost: $0 (all on user's hardware)

## Cost to continue

- GRPO RL training: 2-3 days 3090 time ($0 + electricity)
- Opus tokens to review/debug RL runs: probably 20-30k total
- Evaluation on Colosseum targets: Opus tokens proportional to target count

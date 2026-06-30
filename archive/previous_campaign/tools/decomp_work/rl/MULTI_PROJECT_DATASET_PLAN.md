# Multi-project training dataset plan

**Game-changer**: all GameCube/Wii decomps use CodeWarrior. Mining them
gives us 50,000-100,000 matched (PPC asm → CW C) pairs — enough to
actually train a byte-match decomp model.

## Source projects (all CodeWarrior)

| Project | Repo | Est. matched fns |
|---|---|---|
| Pikmin 1 | https://github.com/projectPiki/pikmin | ~5-10k |
| Pikmin 2 | https://github.com/projectPiki/pikmin2 | ~8-15k |
| Super Smash Bros Melee | https://github.com/doldecomp/melee | ~20k+ |
| Super Mario Sunshine | https://github.com/doldecomp/sms | ~5k |
| Mario Kart Double Dash | https://github.com/doldecomp/mkdd | ~5k |
| Paper Mario TTYD | https://github.com/doldecomp/ttyd | ~3k |
| Kirby Air Ride | https://github.com/doldecomp/kar | ~3k |
| Wii Sports | https://github.com/doldecomp/ogws | ~3k |
| Super Smash Bros Brawl | https://github.com/doldecomp/brawl | ~2k |
| Mario Kart Wii | https://github.com/doldecomp/mkw | ~1k |
| Pokémon Colosseum (this) | local | 160 |
| Pokémon XD | https://github.com/TeamOrre/xd-decomp | ~500 |

**Total: ~50-100k CW-matched function pairs.**

## Pipeline

### 1. Clone all repos to /storage/finetune/decomps/ on 3090

```bash
mkdir -p /storage/finetune/decomps
cd /storage/finetune/decomps
for url in \
    https://github.com/projectPiki/pikmin \
    https://github.com/projectPiki/pikmin2 \
    https://github.com/doldecomp/melee \
    https://github.com/doldecomp/sms \
    https://github.com/doldecomp/mkdd \
    https://github.com/doldecomp/ttyd \
    https://github.com/doldecomp/kar \
    https://github.com/doldecomp/ogws \
    https://github.com/doldecomp/brawl \
    https://github.com/doldecomp/mkw \
    https://github.com/TeamOrre/xd-decomp ; do
    git clone --depth 1 "$url"
done
```

Total disk: probably ~5-10GB.

### 2. Write a per-project extractor

Most of these decomps share a common structure:
- `src/**/*.c` — matched C source
- `asm/**/*.s` OR `build/<flavor>/asm/**/*.s` — raw asm per function
- `configure.py` or `config.yaml` — compile flags, compiler version

Write `tools/decomp_work/rl/extract_multi_project.py` that:

1. Discovers all matched C functions per project (grep `asm void` in C + absence of `#if 1 asm` wrappers = matched)
2. Pairs each C function with its corresponding asm block
3. Determines compile flags/compiler version per file (from configure.py)
4. Emits JSONL record:
   ```json
   {
     "project": "pikmin2",
     "file": "src/plugProjectKandoU/kanban.cpp",
     "fn": "bar_TitleSection__Q24User4kandoF",
     "cw_version": "GC/1.3.2",
     "flags": "-O4,p -fp hard -inline on",
     "asm": "<raw asm, nofralloc-wrapped>",
     "c": "<matched C body>"
   }
   ```

Each project's structure differs — expect 1-2 days to write extractors
covering 5-6 major projects. Pikmin / doldecomp use similar yaml configs.

### 3. Normalize into training format

Every record becomes a (prompt, completion) pair:

- **Prompt**: "Decompile this CW-compiled PowerPC function. Flags: <flags>.\n<asm>\n\nC source:"
- **Completion**: "<c>"

This is a straight SFT dataset. For GRPO/RL, we also need the reward:

- Reward = match% from recompiling completion with the exact flags + comparing bytes
- Needs each project's `configure.py` + binaries (objdiff-cli, mwcceppc) to
  be runnable from the training loop
- Simpler: train as SFT first (teach model the general CW pattern), then
  apply RL later per project for refinement

### 4. Training

**SFT first** (simpler, faster, doesn't need per-project compile):
```bash
python3 train_sft.py \
    --base_model Qwen/Qwen2.5-Coder-7B-Instruct \
    --dataset /storage/finetune/rl/multi_project.jsonl \
    --output /storage/finetune/sft/checkpoints \
    --epochs 2 --lr 1e-5 --lora_r 32
```

- 50-100k examples × 2 epochs × qwen-7b + LoRA = ~24-48 hours on 3090
- Expected: model learns CW codegen patterns broadly, should generalize
  to our specific target far better than zero-shot qwen-32B

**RL refinement** (optional, harder):
- Only feasible on projects where we have the compile toolchain
- Run GRPO with reward = match% per project, specialize for
  pkmn-colosseum's exact CW config

## Honest expectations

With 50k+ training pairs, realistic outcomes:

- **Best case**: fine-tuned 7B hits 70-80% match on unseen CW functions
  zero-shot (vs current qwen-32B's ~30% success rate on complex functions)
- **Realistic**: 50-60% first-try success, much better on simple/medium
  functions, still fails on the hardest (register-allocation-bound) ones
- **Worst case**: model overfits to patterns of dominant projects (Melee)
  and regresses on Colosseum

## Legal / attribution

All listed projects are under open-source licenses (typically MIT or
similar). Training on them is generally fair use but:

- Include attribution in model card / README
- Don't redistribute the raw source of each project in the dataset
  (link back to original repos instead)
- Training output weights are derivative work; check each project's
  license for any redistribution requirements

## Token budget for scaffolding

Writing the multi-project extractor with handling for ~5-6 different
project layouts is ~15-25k Opus tokens. Worth doing separately when
budget allows. The RL scaffolding already committed here supports this
directly — just point `--dataset` at the larger JSONL.

## Priority order

1. **First**: write `extract_pikmin.py` for Pikmin 1/2 (simplest, shared structure)
2. **Second**: add Melee extractor (largest dataset by far)
3. **Third**: SFT training on the combined 30-40k pairs
4. **Fourth**: RL refinement on pkmn-colosseum specifically

Each step can validate before committing to the next. If SFT on Pikmin
data alone already boosts Colosseum match% by 20-30 points, the approach
is validated and worth scaling.

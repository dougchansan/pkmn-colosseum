# LLM Flywheel — next-session runbook (S3 proposer swap)

Status @ 2026-06-23: the autonomous stub farm is built + proven, but the **m2c proposer
yields 0 byte-exact** (~7% compile; drafts semantically imperfect — permuter/opt-sweep
can't fix wrong logic). See KG `project_flywheel_s3_stub_farm_jun23`. The proposer is the
entire gate. This runbook fires the real test the moment the scaled LLM lands.

## 1. Is the scaled SFT done?
```bash
ssh -o BatchMode=yes -i ~/.ssh/id_ed25519 douglaswhittingham@192.168.50.101 \
  'ls -la /storage/finetune/sft/seedcoder8b-cw-scaled/final/adapter_model.safetensors 2>/dev/null; \
   tr "\r" "\n" < /storage/finetune/sft/train_seed_scaled.log | grep -E "SAVED adapter|[0-9]+/3928" | tail -2'
```
Was at step ~471/3928 (~12%), ~34s/it → ETA ~33h from 2026-06-23 ~22:00. Final adapter:
`/storage/finetune/sft/seedcoder8b-cw-scaled/final`.

## 2. Generate LLM drafts (the proposer swap)
`gen_candidate.py` defaults to a Qwen base — the scaled adapter is **Seed-Coder-8B**, so override:
```bash
# on box, per candidate fn: target asm comes from the .inc truth (or objdump target.o)
python3 tools/decomp_work/rl/gen_candidate.py \
  --adapter /storage/finetune/sft/seedcoder8b-cw-scaled/final \
  --base ByteDance-Seed/Seed-Coder-8B-Instruct \
  --asm-file /tmp/<fn>.asm --n 6 --temp 0.8
# -> prints =====CANDIDATE k===== blocks
```
GPU is free once SFT is done; inference is ~seconds/candidate.

## 3. Per-candidate loop (replaces m2c step in run_stub_farm.sh)
For each of the N LLM candidates: `inject_body.py` into a fresh `build_dir.sh` scaffold →
`compile.sh` → if compiles, **opt-level sweep** then permute:
```bash
# opt-sweep (proven KG lever per-fn-opt-level-sweep, permuter-INVISIBLE):
for OPT in 0,p 1,p 2,p 3,p 4,p 0,s 1,s 2,s 3,s 4,s; do
  wibo $MWCC -O$OPT -nodefaults -proc gekko -fp hard -Cpp_exceptions off -enum int -warn off \
       -use_lmw_stmw on -sdata 8 -sdata2 8 -i include -c -o /tmp/sw.o base.c
  score /tmp/sw.o   # Scorer(ign_branch_targets=True, difflib, OD="powerpc-linux-gnu-objdump -dr -EB -mpowerpc -M broadway")
done
# keep best -O base, then: permuter.py . -j8 --stop-on-zero (BUDGET 1800s)
```
Keep the best-scoring compilable candidate; a score-0 from the base/opt-sweep is an immediate
(unverified) win — no permuter needed.

## 4. The two metrics that matter
- **Compile rate**: LLM drafts that build / N. m2c was ~7%. This is the headline — does the LLM
  emit compilable CW C?
- **Semantic correctness / wins**: of compilable, how many reach score 0 (byte-exact) via
  opt-sweep+permuter. This answers whether the LLM generalizes to NEW functions (the research Q).

## 5. ANTI-FRAUD (hard rule)
Every score-0 the farm logs is **UNVERIFIED**. Before any `src/` integration: re-measure
independently in the parent (recompile + Scorer + confirm the `.inc` truth untouched + that the
body is real C, no inline-asm). Only then integrate + record-crack in the KG. See MEMORY.md
`asm-fraud` / `sonnet-halluc` / `inc-tamper` warnings.

## 6. Candidate list
`crackable_stubs.txt` (102 ranked small context-independent stubs). Farm driver: `run_stub_farm.sh`
(add a `PROPOSER=llm` branch that calls §2 instead of m2c_draft, + the §3 opt-sweep). Existing
m2c results in `logs/stub_farm.log` (compiled=2/30, 0 wins) are the baseline to beat.
```

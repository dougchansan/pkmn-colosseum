#!/usr/bin/env python3
"""build_sft_wins.py — build the expert-iteration SFT dataset from the byte-exact
win DB (build/band_wins/*.json). Each example = (target CW-PPC asm -> byte-exact C),
ChatML format, for QLoRA SFT of Qwen2.5-Coder-7B-Base.

Asm is read from the one-time dtk disasm of the canonical target object (cached),
with a per-fn .inc fallback. Only >=100% matches (everything in band_wins) are used.
"""
import os
import json, glob, re, subprocess, tempfile, statistics
from pathlib import Path

ROOT = Path("C:/Users/douglaswhittingham/pkmn-colosseum")
TARGET = ROOT / "build/GC6E01/obj/auto_01_800055E0_text.o"
DTK = ROOT / ("tools/dtk.exe" if os.name == "nt" else "tools/dtk")
CACHE = Path(tempfile.gettempdir()) / "auto_01_disasm.txt"
OUT = ROOT / "tools/decomp_work/rl/sft_wins.jsonl"
OUT.parent.mkdir(parents=True, exist_ok=True)

SYS = ("You are a GameCube PowerPC decompiler. Convert the target CodeWarrior 1.3 "
       "PPC assembly to byte-matching C89. Use extern labels for SDA globals, never "
       "float literals for sdata2 returns. Output only the C function with any needed externs.")
META_KEYS = {"_src", "_srcs", "_pct"}

# 1) collect unique win functions {fn: C}
wins = {}
for f in glob.glob(str(ROOT / "build/band_wins/*.json")):
    try:
        d = json.load(open(f, encoding="utf-8"))
    except Exception:
        continue
    for k, v in d.items():
        if k in META_KEYS:
            continue
        if isinstance(v, str) and v.strip():
            wins.setdefault(k, v)
print(f"collected {len(wins)} unique byte-exact win functions")

# 2) ensure one-time disasm cache
if not CACHE.exists() or CACHE.stat().st_size < 1000:
    print("disassembling target object once (dtk)...")
    subprocess.run([str(DTK), "elf", "disasm", str(TARGET), str(CACHE)], cwd=str(ROOT))
print(f"disasm cache: {CACHE} ({CACHE.stat().st_size if CACHE.exists() else 0} bytes)")

# 3) parse cache into {fn: asm-block}
asm = {}
cur, buf = None, []
for ln in CACHE.read_text(errors="replace").splitlines():
    m = re.match(r"^\.fn\s+(\S+)", ln)
    if m:
        cur = m.group(1).rstrip(","); buf = [ln]; continue
    if cur:
        buf.append(ln)
        if re.match(rf"^\.endfn\s+{re.escape(cur)}\b", ln):
            asm[cur] = "\n".join(buf); cur = None
print(f"parsed asm for {len(asm)} functions from cache")

def from_inc(fn):
    for p in (ROOT / "src/game").glob(f"*_{fn}.inc"):
        return p.read_text(errors="replace")
    return None

# 4) build ChatML examples
examples, skipped = [], []
for fn, c in wins.items():
    a = asm.get(fn) or from_inc(fn)
    if not a:
        skipped.append(fn); continue
    examples.append({"messages": [
        {"role": "system", "content": SYS},
        {"role": "user", "content": f"Decompile this function to byte-matching C:\n\n```\n{a.strip()}\n```"},
        {"role": "assistant", "content": f"```c\n{c.strip()}\n```"},
    ]})

with open(OUT, "w", encoding="utf-8") as f:
    for e in examples:
        f.write(json.dumps(e, ensure_ascii=False) + "\n")

# 5) stats
tl = [(len(e["messages"][1]["content"]) + len(e["messages"][2]["content"])) // 4 for e in examples]
print(f"\nWROTE {len(examples)} examples -> {OUT}")
print(f"skipped (no asm found): {len(skipped)}  e.g. {skipped[:5]}")
if tl:
    print(f"approx token lengths: min {min(tl)} median {int(statistics.median(tl))} "
          f"max {max(tl)} | >8192: {sum(1 for t in tl if t > 8192)}")
    print("\n--- sample example[0] (truncated) ---")
    s = examples[0]
    print("USER:", s["messages"][1]["content"][:280])
    print("ASSISTANT:", s["messages"][2]["content"][:220])

#!/usr/bin/env python3
"""
Decomp Model Benchmark

Tests local models on our known-good decomp examples to find the best
model for PPC asm → C89 decompilation matching.

Usage:
    python tools/decomp_benchmark.py --build-suite       # Create test suite from matches
    python tools/decomp_benchmark.py --run MODEL         # Benchmark a specific model
    python tools/decomp_benchmark.py --run-all           # Benchmark all available models
    python tools/decomp_benchmark.py --leaderboard       # Show results
    python tools/decomp_benchmark.py --export-finetune   # Export LoRA fine-tuning dataset
"""

import argparse
import json
import os
import re
import subprocess
import sys
import time
import urllib.request
from pathlib import Path

ROOT = Path(__file__).parent.parent
BENCH_DIR = ROOT / "tools" / "decomp_work" / "benchmark"
SUITE_FILE = BENCH_DIR / "test_suite.json"
RESULTS_DIR = BENCH_DIR / "results"
FINETUNE_DIR = BENCH_DIR / "finetune"

OLLAMA_HOST = os.environ.get("DECOMP_GPU_HOST", "192.168.50.101")
OLLAMA_PORT = int(os.environ.get("DECOMP_OLLAMA_PORT", "11434"))

SDA_BASE = 0x80480820
SDA2_BASE = 0x804836A0

# Known-good matches: (function_name, file, inc_file, correct_c_code)
# We'll build these from the actual codebase
KNOWN_MATCHES = [
    # Pattern: getter
    ("fn_800D3094", "src/game/gs_gfx.c",
     "src/game/gs_gfx_fn_800D3094.inc",
     'extern u32 lbl_8047AA80;\nu32 fn_800D3094(void) {\n    return *(u32*)((u8*)lbl_8047AA80 + 0x4C);\n}'),
    # Pattern: setter
    ("fn_800D30A0", "src/game/gs_gfx.c",
     "src/game/gs_gfx_fn_800D30A0.inc",
     'extern u32 lbl_8047AA80;\nvoid fn_800D30A0(u32 val) {\n    *(u32*)((u8*)lbl_8047AA80 + 0x48) = val;\n}'),
    # Pattern: float return (sdata2)
    ("fn_801AE000", "src/hsd/hsd_pobj.c",
     "src/hsd/hsd_pobj_ext_fn_801AE000.inc",
     'extern f32 lbl_8047DD60;\nf32 fn_801AE000(void) {\n    return lbl_8047DD60;\n}'),
    # Pattern: dual global store
    ("fn_80163F88", "src/game/people/people_field.c",
     "src/game/people/people_field_fn_80163F88.inc",
     'extern u32 lbl_8047B098;\nextern u32 lbl_8047B088;\nvoid fn_80163F88(void) {\n    lbl_8047B098 = 1;\n    lbl_8047B088 = 1;\n}'),
    # Pattern: output params
    ("fn_800140FC", "src/game/gs_event_exec.c",
     "src/game/gs_event_exec_fn_800140FC.inc",
     'extern u32 lbl_8047A2F8;\nextern u32 lbl_8047A2F4;\nvoid fn_800140FC(u32* out1, u32* out2) {\n    *out1 = lbl_8047A2F8;\n    *out2 = lbl_8047A2F4;\n}'),
    # Pattern: input setter with return 0
    ("fn_800F75FC", "src/game/input/input.c",
     "src/game/input/input_fn_800F75FC.inc",
     'extern u8* lbl_80478B00;\ns32 fn_800F75FC(u32 val) {\n    *(u32*)(lbl_80478B00 + 0x10) = val;\n    return 0;\n}'),
    # Pattern: thunk (pass-through)
    ("fn_801C6908", "src/game/battle/battle_scene.c",
     "src/game/battle/battle_scene_fn_801C6908.inc",
     'extern void fn_800E0C04(void);\nvoid fn_801C6908(void) {\n    fn_800E0C04();\n}'),
    # Pattern: thunk with arg
    ("fn_801C673C", "src/game/battle/battle_scene.c",
     "src/game/battle/battle_scene_fn_801C673C.inc",
     'extern void fn_800D9ED8(u32);\n#pragma peephole off\nvoid fn_801C673C(void) {\n    fn_800D9ED8(0);\n}\n#pragma peephole on'),
    # Pattern: conditional float
    ("fn_8018F5B4", "src/game/people/people.c",
     "src/game/people/people_fn_8018F5B4.inc",
     'extern f32 lbl_8047D8A8;\nf32 fn_8018F5B4(u8* ptr) {\n    if (ptr != NULL) {\n        return *(f32*)(ptr + 0x14);\n    }\n    return lbl_8047D8A8;\n}'),
    # Pattern: conditional float (variant)
    ("fn_8018F5CC", "src/game/people/people.c",
     "src/game/people/people_fn_8018F5CC.inc",
     'extern f32 lbl_8047D8A8;\nf32 fn_8018F5CC(u8* ptr) {\n    if (ptr != NULL) {\n        return *(f32*)(ptr + 0x10);\n    }\n    return lbl_8047D8A8;\n}'),
    # Pattern: block-scoped double load
    ("fn_80162858", "src/game/people/people_field.c",
     "src/game/people/people_field_fn_80162858.inc",
     'void fn_80162858(u32 index, u32 val1, u32 val2) {\n    extern u32 lbl_8047B024;\n    u32 offset = index * 0xF4;\n    {\n        u8* elem1 = (u8*)lbl_8047B024 + offset;\n        *(u32*)(elem1 + 0x94) = val1;\n    }\n    {\n        u8* elem2 = (u8*)lbl_8047B024 + offset;\n        *(u32*)(elem2 + 0x98) = val2;\n    }\n}'),
    # Pattern: aligned subtraction
    ("fn_80163BCC", "src/game/people/people_field.c",
     "src/game/people/people_field_fn_80163BCC.inc",
     'extern u32 lbl_8047B078;\nvoid fn_80163BCC(u8* unused, u32 size) {\n    lbl_8047B078 -= (size + 0x1F) & ~0x1F;\n}'),
    # Pattern: thunk (fsys)
    ("fn_8017B1AC", "src/game/fsys/fsys_file.c",
     "src/game/gs_scene_fn_8017B1AC.inc",
     'extern void fn_80167E34(void);\nvoid fn_8017B1AC(void) {\n    fn_80167E34();\n}'),
]


def ensure_dirs():
    for d in [BENCH_DIR, RESULTS_DIR, FINETUNE_DIR]:
        d.mkdir(parents=True, exist_ok=True)


def build_prompt(fn_name, inc_content, c_file_path):
    """Build the same prompt the agent would use."""
    # Read context from C file
    c_content = (ROOT / c_file_path).read_text(encoding="utf-8", errors="replace")
    c_lines = c_content.split("\n")

    # Find the function in the file
    fn_line = -1
    for i, line in enumerate(c_lines):
        if fn_name in line and ("asm void" in line or f"void {fn_name}" in line
                                or f"u32 {fn_name}" in line or f"f32 {fn_name}" in line):
            fn_line = i
            break

    context_start = max(0, fn_line - 10) if fn_line >= 0 else 0
    context_end = min(len(c_lines), fn_line + 15) if fn_line >= 0 else 30
    context = "\n".join(c_lines[context_start:context_end])

    # SDA hints
    sda_hints = []
    for m in re.finditer(r'(?:lwz|stw|lfs|lbz|lhz|sth|stb)\s+r\d+,\s*(-?\d+)\(r13\)', inc_content):
        offset = int(m.group(1))
        addr = SDA_BASE + offset
        sda_hints.append(f"  r13 offset {offset} -> lbl_{addr:08X}")
    for m in re.finditer(r'(?:lfs|lfd)\s+f\d+,\s*(-?\d+)\(r2\)', inc_content):
        offset = int(m.group(1))
        addr = SDA2_BASE + offset
        sda_hints.append(f"  r2 offset {offset} -> lbl_{addr:08X} (sdata2 float, use extern f32)")
    sda_section = "\n".join(sda_hints) if sda_hints else "  (none)"

    prompt = f"""You are decompiling a GameCube (PowerPC) function for Pokemon Colosseum.
Convert the following PPC assembly into byte-matching C89 code.

CRITICAL RULES:
- C89 only: ALL declarations before statements in each block
- Use block scoping {{ }} when the asm loads the SAME global from r13 twice
- NEVER use float literals (0.0f) — use `extern f32 lbl_XXXXXXXX;`
- SDA_BASE = 0x80480820 (r13), SDA2_BASE = 0x804836A0 (r2)
- Leaf functions (nofralloc, no bl) need NO pragmas — default O4
- Thunks (stwu/mflr/bl/epilogue) = `extern void target(); void fn() {{ target(); }}`
- `subf rD, rA, rB` = rB - rA (reversed!)
- `clrrwi rD, rS, N` = rS & ~((1<<N)-1)
- Output ONLY the C function + needed externs. No asm, no #if blocks, no explanation.

EXAMPLE - Global Pointer Getter:
ASM: `lwz r3, sym(r13) / lwz r3, 0x4c(r3) / blr`
C: `extern u32 sym; u32 fn(void) {{ return *(u32*)((u8*)sym + 0x4C); }}`

EXAMPLE - Float Return:
ASM: `lfs f1, offset(r2) / blr`
C: `extern f32 lbl_XXX; f32 fn(void) {{ return lbl_XXX; }}`

EXAMPLE - Thunk:
ASM: `stwu/mflr/stw/bl fn_XXX/lwz/mtlr/addi/blr`
C: `extern void fn_XXX(void); void fn(void) {{ fn_XXX(); }}`

FUNCTION: {fn_name}

ASSEMBLY:
```
{inc_content}
```

SDA ADDRESS HINTS:
{sda_section}

CONTEXT:
```c
{context}
```

Write the C89 replacement function."""
    return prompt


def call_ollama(prompt, model, timeout=120):
    """Call Ollama API."""
    url = f"http://{OLLAMA_HOST}:{OLLAMA_PORT}/api/generate"
    payload = json.dumps({
        "model": model,
        "prompt": prompt,
        "stream": False,
        "options": {"temperature": 0.1, "num_predict": 2048}
    }).encode()

    req = urllib.request.Request(url, data=payload,
                                headers={"Content-Type": "application/json"})
    try:
        with urllib.request.urlopen(req, timeout=timeout) as resp:
            result = json.loads(resp.read())
            return result.get("response", "")
    except Exception as e:
        return f"ERROR: {e}"


def extract_c_code(response):
    """Extract C code from model response."""
    if not response or response.startswith("ERROR:"):
        return None
    m = re.search(r'```c?\s*\n(.*?)```', response, re.DOTALL)
    if m:
        return m.group(1).strip()
    if "void " in response or "return " in response or "extern " in response:
        lines = response.split("\n")
        for i, line in enumerate(lines):
            if line.strip().startswith(("extern ", "void ", "u32 ", "s32 ",
                                        "f32 ", "u8 ", "#pragma ", "static ")):
                return "\n".join(lines[i:]).strip()
    return None


def score_response(fn_name, response, correct_c):
    """Score a model's response against the known-correct answer."""
    c_code = extract_c_code(response)

    result = {
        "extracted": c_code is not None,
        "compile": False,
        "match_pct": 0.0,
        "correct_structure": False,
    }

    if c_code is None:
        return result

    # Check structural similarity (does it have the right externs, function name, etc.)
    result["correct_structure"] = fn_name in c_code

    # Check for common anti-patterns
    if "0.0f" in c_code and "extern f32" in correct_c:
        result["anti_pattern"] = "used float literal instead of extern label"
    if "asm" in c_code.lower() and "pragma" not in c_code.lower():
        result["anti_pattern"] = "included raw asm in C code"

    # Key pattern checks
    if "extern f32" in correct_c:
        result["uses_extern_float"] = "extern f32" in c_code
    if "{" in correct_c and correct_c.count("{") > 2:
        result["uses_block_scope"] = c_code.count("{") >= correct_c.count("{")

    return result


def cmd_build_suite(args):
    """Build test suite from known matches."""
    ensure_dirs()
    suite = []

    for fn_name, c_file, inc_file, correct_c in KNOWN_MATCHES:
        inc_path = ROOT / inc_file
        if not inc_path.exists():
            print(f"SKIP {fn_name}: {inc_file} not found")
            continue

        inc_content = inc_path.read_text(encoding="utf-8", errors="replace")
        prompt = build_prompt(fn_name, inc_content, c_file)

        suite.append({
            "function": fn_name,
            "file": c_file,
            "inc_file": inc_file,
            "asm": inc_content,
            "correct_c": correct_c,
            "prompt": prompt,
            "asm_lines": len([l for l in inc_content.strip().split("\n")
                            if l.strip() and "nofralloc" not in l]),
        })

    SUITE_FILE.write_text(json.dumps(suite, indent=2), encoding="utf-8")
    print(f"Built test suite: {len(suite)} functions")
    for s in suite:
        print(f"  {s['function']:20s} ({s['asm_lines']:2d} lines) - {s['file']}")


def cmd_run(args):
    """Run benchmark on a specific model."""
    ensure_dirs()
    if not SUITE_FILE.exists():
        print("No test suite. Run --build-suite first.")
        return

    suite = json.loads(SUITE_FILE.read_text(encoding="utf-8"))
    model = args.run

    print(f"\n{'='*60}")
    print(f"BENCHMARKING: {model}")
    print(f"Test functions: {len(suite)}")
    print(f"{'='*60}\n")

    # Check model is available
    try:
        url = f"http://{OLLAMA_HOST}:{OLLAMA_PORT}/api/tags"
        with urllib.request.urlopen(url, timeout=10) as resp:
            models = json.loads(resp.read())
            available = [m["name"] for m in models.get("models", [])]
            if model not in available and f"{model}:latest" not in available:
                print(f"Model '{model}' not available. Available: {available}")
                return
    except Exception as e:
        print(f"Cannot reach Ollama: {e}")
        return

    results = []
    total_time = 0

    for i, test in enumerate(suite):
        fn = test["function"]
        print(f"[{i+1}/{len(suite)}] {fn} ({test['asm_lines']} lines)...", end=" ", flush=True)

        start = time.time()
        response = call_ollama(test["prompt"], model, timeout=180)
        elapsed = time.time() - start
        total_time += elapsed

        score = score_response(fn, response, test["correct_c"])
        score["function"] = fn
        score["time"] = round(elapsed, 1)
        score["response"] = response[:500] if response else ""

        c_code = extract_c_code(response)

        # Quick structural check
        status = "FAIL"
        if c_code and test["function"] in c_code:
            if "asm" not in c_code.lower() or "pragma" in c_code.lower():
                status = "PASS"
                if score.get("anti_pattern"):
                    status = "WARN"
        elif c_code:
            status = "PARTIAL"

        score["status"] = status
        results.append(score)
        print(f"{status} ({elapsed:.1f}s)")

    # Save results
    result_file = RESULTS_DIR / f"{model.replace(':', '_').replace('/', '_')}.json"
    result_file.write_text(json.dumps({
        "model": model,
        "timestamp": time.time(),
        "total_time": round(total_time, 1),
        "results": results,
    }, indent=2), encoding="utf-8")

    # Print summary
    passed = sum(1 for r in results if r["status"] == "PASS")
    warned = sum(1 for r in results if r["status"] == "WARN")
    failed = sum(1 for r in results if r["status"] in ("FAIL", "PARTIAL"))
    extracted = sum(1 for r in results if r["extracted"])

    print(f"\n{'='*60}")
    print(f"RESULTS: {model}")
    print(f"{'='*60}")
    print(f"  Extracted C code:  {extracted}/{len(results)}")
    print(f"  Correct structure: {passed + warned}/{len(results)}")
    print(f"  PASS:  {passed}")
    print(f"  WARN:  {warned} (anti-patterns like float literals)")
    print(f"  FAIL:  {failed}")
    print(f"  Total time: {total_time:.1f}s ({total_time/len(results):.1f}s/function)")
    print(f"  Results saved: {result_file}")
    print(f"{'='*60}")


def cmd_run_all(args):
    """Benchmark all available Ollama models."""
    try:
        url = f"http://{OLLAMA_HOST}:{OLLAMA_PORT}/api/tags"
        with urllib.request.urlopen(url, timeout=10) as resp:
            models = json.loads(resp.read())
            available = [m["name"] for m in models.get("models", [])]
    except Exception as e:
        print(f"Cannot reach Ollama: {e}")
        return

    print(f"Available models: {available}")
    for model in available:
        args.run = model
        cmd_run(args)
        print()


def cmd_leaderboard(args):
    """Show benchmark leaderboard."""
    ensure_dirs()
    if not RESULTS_DIR.exists():
        print("No results yet. Run --run MODEL first.")
        return

    entries = []
    for result_file in RESULTS_DIR.glob("*.json"):
        data = json.loads(result_file.read_text(encoding="utf-8"))
        results = data["results"]
        passed = sum(1 for r in results if r["status"] == "PASS")
        warned = sum(1 for r in results if r["status"] == "WARN")
        total = len(results)
        avg_time = data["total_time"] / total if total > 0 else 0

        entries.append({
            "model": data["model"],
            "pass": passed,
            "warn": warned,
            "total": total,
            "score": (passed + warned * 0.5) / total * 100 if total > 0 else 0,
            "avg_time": avg_time,
        })

    entries.sort(key=lambda x: -x["score"])

    print(f"\n{'='*70}")
    print(f"DECOMP MODEL LEADERBOARD")
    print(f"{'='*70}")
    print(f"{'Rank':>4s}  {'Model':<30s}  {'Score':>6s}  {'Pass':>4s}  {'Warn':>4s}  {'Avg Time':>8s}")
    print(f"{'-'*70}")
    for i, e in enumerate(entries):
        print(f"{i+1:4d}  {e['model']:<30s}  {e['score']:5.1f}%  {e['pass']:4d}  {e['warn']:4d}  {e['avg_time']:7.1f}s")
    print(f"{'='*70}")


def cmd_export_finetune(args):
    """Export fine-tuning dataset in chat format for LoRA training."""
    ensure_dirs()
    if not SUITE_FILE.exists():
        print("No test suite. Run --build-suite first.")
        return

    suite = json.loads(SUITE_FILE.read_text(encoding="utf-8"))

    # ChatML format for fine-tuning
    chatml_data = []
    for test in suite:
        chatml_data.append({
            "messages": [
                {
                    "role": "system",
                    "content": "You are a GameCube PPC decompiler. Convert PowerPC assembly to matching C89 code. Use extern labels for SDA globals, never float literals. Output only C code."
                },
                {
                    "role": "user",
                    "content": test["prompt"]
                },
                {
                    "role": "assistant",
                    "content": f"```c\n{test['correct_c']}\n```"
                }
            ]
        })

    # JSONL format
    jsonl_file = FINETUNE_DIR / "decomp_train.jsonl"
    with open(jsonl_file, "w", encoding="utf-8") as f:
        for entry in chatml_data:
            f.write(json.dumps(entry, ensure_ascii=False) + "\n")

    # Also export as Alpaca format (instruction/input/output)
    alpaca_data = []
    for test in suite:
        alpaca_data.append({
            "instruction": "Convert this GameCube PowerPC assembly to matching C89 code. Use extern labels for SDA globals (r13-relative). Never use float literals for sdata2 returns. Output only the C function with needed externs.",
            "input": test["asm"],
            "output": test["correct_c"],
        })

    alpaca_file = FINETUNE_DIR / "decomp_train_alpaca.json"
    alpaca_file.write_text(json.dumps(alpaca_data, indent=2, ensure_ascii=False),
                          encoding="utf-8")

    print(f"Exported fine-tuning datasets:")
    print(f"  ChatML (JSONL): {jsonl_file} ({len(chatml_data)} examples)")
    print(f"  Alpaca (JSON):  {alpaca_file} ({len(alpaca_data)} examples)")
    print(f"\nTo fine-tune with LLMFit on the 3090:")
    print(f"  1. Copy {FINETUNE_DIR} to {OLLAMA_HOST}:/storage/finetune/")
    print(f"  2. Run LLMFit with qwen2.5-coder:7b as base")
    print(f"  3. Train on decomp_train.jsonl")
    print(f"  4. Import the LoRA adapter back into Ollama")


def main():
    parser = argparse.ArgumentParser(description="Decomp Model Benchmark")
    parser.add_argument("--build-suite", action="store_true",
                       help="Build test suite from known matches")
    parser.add_argument("--run", metavar="MODEL",
                       help="Benchmark a specific Ollama model")
    parser.add_argument("--run-all", action="store_true",
                       help="Benchmark all available models")
    parser.add_argument("--leaderboard", action="store_true",
                       help="Show results leaderboard")
    parser.add_argument("--export-finetune", action="store_true",
                       help="Export LoRA fine-tuning dataset")

    args = parser.parse_args()

    if args.build_suite:
        cmd_build_suite(args)
    elif args.run:
        cmd_run(args)
    elif args.run_all:
        cmd_run_all(args)
    elif args.leaderboard:
        cmd_leaderboard(args)
    elif args.export_finetune:
        cmd_export_finetune(args)
    else:
        parser.print_help()


if __name__ == "__main__":
    main()

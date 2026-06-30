#!/usr/bin/env python3
"""
Decomp Task Scheduler & Load Balancer

Scans all remaining #if 1 asm wrappers, classifies by complexity,
generates self-contained decomp prompts, and routes to the appropriate
model backend (free cloud, local GPU, or Claude).

Usage:
    python tools/decomp_scheduler.py --scan          # Scan wrappers; write report only
    python tools/decomp_scheduler.py --scan --update-queue  # Rewrite work_queue.json
    python tools/decomp_scheduler.py --generate N    # Generate N task prompts
    python tools/decomp_scheduler.py --status        # Show progress
    python tools/decomp_scheduler.py --verify FILE   # Verify a decompiled function
"""

import argparse
import datetime
import json
import os
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).parent.parent
WORK_DIR = ROOT / "tools" / "decomp_work"
QUEUE_FILE = WORK_DIR / "work_queue.json"
PROGRESS_FILE = WORK_DIR / "progress.json"
LOCK_DIR = WORK_DIR / "locks"
PROMPTS_DIR = WORK_DIR / "prompts"
REPORTS_DIR = WORK_DIR / "reports"

SDA_BASE = 0x80480820
SDA2_BASE = 0x804836A0

# Complexity tiers
TIER_SIMPLE = "simple"      # 2-10 asm lines -> free cloud / local GPU
TIER_MEDIUM = "medium"      # 11-30 asm lines -> local GPU / Claude
TIER_COMPLEX = "complex"    # 31-80 asm lines -> Claude
TIER_HARD = "hard"          # 80+ asm lines -> Claude only

TIER_LIMITS = {
    TIER_SIMPLE: (0, 10),
    TIER_MEDIUM: (11, 30),
    TIER_COMPLEX: (31, 80),
    TIER_HARD: (81, 99999),
}

TIER_BACKENDS = {
    TIER_SIMPLE: ["ollama", "kimi", "opencode", "claude"],
    TIER_MEDIUM: ["ollama", "kimi", "claude"],
    TIER_COMPLEX: ["claude"],
    TIER_HARD: ["claude"],
}


def ensure_dirs():
    for d in [WORK_DIR, LOCK_DIR, PROMPTS_DIR, REPORTS_DIR]:
        d.mkdir(parents=True, exist_ok=True)


def scan_wrappers():
    """Find all remaining #if 1 asm wrappers and their .inc files."""
    wrappers = []
    src_dir = ROOT / "src"

    for c_file in src_dir.rglob("*.c"):
        rel_path = c_file.relative_to(ROOT)
        content = c_file.read_text(encoding="utf-8", errors="replace")
        lines = content.split("\n")

        i = 0
        while i < len(lines):
            line = lines[i].strip()
            if line == "#if 1" and i + 1 < len(lines):
                next_line = lines[i + 1].strip()
                m = re.match(r'asm void (\w+)\(void\)\s*\{', next_line)
                if m:
                    fn_name = m.group(1)
                    # Find the .inc file
                    inc_match = None
                    if i + 2 < len(lines):
                        inc_line = lines[i + 2].strip()
                        inc_m = re.match(r'#include "([^"]+)"', inc_line)
                        if inc_m:
                            inc_match = inc_m.group(1)

                    if inc_match:
                        inc_path = ROOT / inc_match
                        if inc_path.exists():
                            inc_content = inc_path.read_text(encoding="utf-8", errors="replace")
                            asm_lines = [l for l in inc_content.strip().split("\n")
                                        if l.strip() and not l.strip().startswith("nofralloc")]
                            line_count = len(asm_lines)
                        else:
                            line_count = 0
                    else:
                        line_count = 0

                    # Determine surrounding pragmas
                    pragmas = []
                    for j in range(max(0, i - 5), i):
                        pl = lines[j].strip()
                        if pl.startswith("#pragma"):
                            pragmas.append(pl)

                    # Classify complexity
                    tier = TIER_SIMPLE
                    for t, (lo, hi) in TIER_LIMITS.items():
                        if lo <= line_count <= hi:
                            tier = t
                            break

                    wrappers.append({
                        "function": fn_name,
                        "file": str(rel_path).replace("\\", "/"),
                        "inc_file": inc_match,
                        "asm_lines": line_count,
                        "tier": tier,
                        "line_number": i + 1,
                        "pragmas": pragmas,
                        "status": "pending",
                    })
            i += 1

    return wrappers


def load_progress():
    if PROGRESS_FILE.exists():
        return json.loads(PROGRESS_FILE.read_text())
    return {"completed": [], "failed": [], "in_progress": []}


def save_progress(progress):
    PROGRESS_FILE.write_text(json.dumps(progress, indent=2))


def load_queue():
    if QUEUE_FILE.exists():
        return json.loads(QUEUE_FILE.read_text())
    return []


def save_queue(queue):
    QUEUE_FILE.write_text(json.dumps(queue, indent=2))


def save_scan_report(wrappers, report_path=None):
    """Write scan output to a separate report file, not the work queue."""
    if report_path:
        path = Path(report_path)
        if not path.is_absolute():
            path = ROOT / path
    else:
        stamp = datetime.datetime.now().strftime("%Y%m%d-%H%M%S")
        path = REPORTS_DIR / f"work_queue_scan-{stamp}.json"

    by_tier = {}
    by_file = {}
    for w in wrappers:
        by_tier[w["tier"]] = by_tier.get(w["tier"], 0) + 1
        by_file[w["file"]] = by_file.get(w["file"], 0) + 1

    report = {
        "generated_at": datetime.datetime.now().isoformat(timespec="seconds"),
        "total_remaining_wrappers": len(wrappers),
        "by_tier": by_tier,
        "by_file": dict(sorted(by_file.items(), key=lambda kv: (-kv[1], kv[0]))),
        "wrappers": wrappers,
    }
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(report, indent=2), encoding="utf-8")
    return path


def acquire_lock(fn_name):
    lock_file = LOCK_DIR / f"{fn_name}.lock"
    if lock_file.exists():
        return False
    lock_file.write_text(f"locked by pid {os.getpid()}")
    return True


def release_lock(fn_name):
    lock_file = LOCK_DIR / f"{fn_name}.lock"
    if lock_file.exists():
        lock_file.unlink()


def detect_pattern(inc_content):
    """Detect common asm patterns to select the best few-shot example."""
    lines = [l.strip() for l in inc_content.strip().split("\n")
             if l.strip() and not l.strip().startswith("nofralloc")]

    has_bl = any("bl " in l and "blr" not in l for l in lines)
    has_lfs = any("lfs " in l for l in lines)
    has_stw_sda = any(re.search(r'stw r\d+,.*\(r13\)', l) for l in lines)
    has_lwz_sda = any(re.search(r'lwz r\d+,.*\(r13\)', l) for l in lines)
    has_stwu = any("stwu " in l for l in lines)
    has_lfs_r2 = any(re.search(r'lfs f\d+,.*\(r2\)', l) for l in lines)
    has_cmplwi_0 = any("cmplwi" in l and "0x0" in l for l in lines)
    has_clrrwi = any("clrrwi" in l for l in lines)
    sda_loads = sum(1 for l in lines if re.search(r'lwz r\d+,.*\(r13\)', l))

    # Thunk: just stwu/mflr + bl + epilogue
    if has_stwu and has_bl and len(lines) <= 8:
        return "thunk"
    # Float return from sdata2
    if has_lfs_r2 and not has_bl and len(lines) <= 4:
        return "float_return"
    # Conditional float getter
    if has_cmplwi_0 and has_lfs:
        return "conditional_float"
    # Simple getter (load from global pointer)
    if has_lwz_sda and not has_stw_sda and not has_bl and len(lines) <= 5:
        return "getter"
    # Simple setter
    if has_stw_sda and not has_bl and len(lines) <= 5:
        return "setter"
    # Dual store (same value to two globals)
    if has_stw_sda and lines.count(lines[0]) == 1 and sda_loads == 0 and len(lines) <= 5:
        return "dual_store"
    # Output parameters
    if has_lwz_sda and "0x0(r3)" in inc_content and "0x0(r4)" in inc_content:
        return "output_params"
    # Block-scoped double load
    if sda_loads >= 2:
        return "double_load"
    # Aligned arithmetic
    if has_clrrwi:
        return "aligned_math"

    return "generic"


PATTERN_EXAMPLES = {
    "thunk": """## Relevant Example: Pass-Through Thunk
ASM: stwu/mflr/bl fn_XXX/lwz/mtlr/addi/blr
C: `extern void fn_XXX(void); void fn_YYY(void) { fn_XXX(); }`
The prologue/epilogue is just call overhead. No pragmas needed.""",

    "float_return": """## Relevant Example: sdata2 Float Return
ASM: lfs f1, OFFSET(r2) / blr
C: `extern f32 lbl_XXXXXXXX; f32 fn(void) { return lbl_XXXXXXXX; }`
CRITICAL: NEVER use float literals (0.0f, 1.0f). ALWAYS use extern label.
Compute address: SDA2_BASE(0x804836A0) + sign_extend(offset).""",

    "conditional_float": """## Relevant Example: Conditional Float Getter
ASM: cmplwi r3,0 / beq @null / lfs f1,OFFSET(r3) / blr / @null: lfs f1,OFFSET(r2) / blr
C:
```c
extern f32 lbl_XXXXXXXX;
f32 fn(u8* ptr) {
    if (ptr != NULL) { return *(f32*)(ptr + OFFSET); }
    return lbl_XXXXXXXX;
}
```""",

    "getter": """## Relevant Example: Global Pointer Getter
ASM: lwz r3, sym(r13) / lwz r3, OFFSET(r3) / blr
C: `extern u32 sym; u32 fn(void) { return *(u32*)((u8*)sym + OFFSET); }`""",

    "setter": """## Relevant Example: Global Pointer Setter
ASM: lwz r4, sym(r13) / stw r3, OFFSET(r4) / blr
C: `extern u32 sym; void fn(u32 val) { *(u32*)((u8*)sym + OFFSET) = val; }`""",

    "output_params": """## Relevant Example: Output Parameters
ASM: lwz r0, sym1(r13) / stw r0, 0(r3) / lwz r0, sym2(r13) / stw r0, 0(r4) / blr
C: `void fn(u32* out1, u32* out2) { *out1 = sym1; *out2 = sym2; }`""",

    "double_load": """## Relevant Example: Block-Scoped Double Global Load
When asm loads a global from r13 TWICE (two separate lwz of same symbol),
use separate { } blocks to prevent CW from CSE-ing them:
```c
void fn(u32 idx, u32 v1, u32 v2) {
    extern u32 g;
    u32 off = idx * SIZE;
    { u8* p = (u8*)g + off; *(u32*)(p + 0x94) = v1; }
    { u8* p = (u8*)g + off; *(u32*)(p + 0x98) = v2; }
}
```""",

    "aligned_math": """## Relevant Example: Aligned Size Math
clrrwi rD, rS, 5 = (value) & ~0x1F (align down to 32)
addi + clrrwi = (value + 0x1F) & ~0x1F (round UP to 32)
subf rD, rA, rB = rB - rA (note reversed operands!)""",
}


def generate_prompt(wrapper, retry_diff=None):
    """Generate a self-contained decomp prompt for a single function."""
    inc_path = ROOT / wrapper["inc_file"]
    if not inc_path.exists():
        return None

    inc_content = inc_path.read_text(encoding="utf-8", errors="replace")

    # Read surrounding context from the C file
    c_file = ROOT / wrapper["file"]
    c_content = c_file.read_text(encoding="utf-8", errors="replace")
    c_lines = c_content.split("\n")
    ln = wrapper["line_number"] - 1
    context_start = max(0, ln - 15)
    context_end = min(len(c_lines), ln + 20)
    context = "\n".join(c_lines[context_start:context_end])

    # Detect pattern and get relevant example
    pattern = detect_pattern(inc_content)
    example = PATTERN_EXAMPLES.get(pattern, "")

    # Compute SDA addresses for any r13/r2 references
    sda_hints = []
    for m in re.finditer(r'(?:lwz|stw|lfs|lbz|lhz|sth|stb)\s+r\d+,\s*(-?\d+)\(r13\)', inc_content):
        offset = int(m.group(1))
        addr = SDA_BASE + offset
        sda_hints.append(f"  r13 offset {offset} -> 0x{addr:08X} (look up lbl_{{:08X}} in symbols)")
    for m in re.finditer(r'(?:lfs|lfd)\s+f\d+,\s*(-?\d+)\(r2\)', inc_content):
        offset = int(m.group(1))
        addr = SDA2_BASE + offset
        sda_hints.append(f"  r2 offset {offset} -> 0x{addr:08X} (sdata2 float, use extern f32 lbl_{{:08X}})")
    sda_section = "\n".join(sda_hints) if sda_hints else "  (none)"

    # Build retry section if this is a second attempt
    retry_section = ""
    if retry_diff:
        retry_section = f"""
PREVIOUS ATTEMPT FAILED. Here is the instruction diff (LEFT=target, RIGHT=yours):
```
{retry_diff}
```
Fix the mismatches. Common fixes:
- Wrong register → reorder declarations or change variable types
- Extra li instructions → remove unnecessary pragmas, let O4 CSE
- lis/addi vs SDA → change extern u8[] to extern u32 scalar
- Wrong stack frame → try different #pragma optimization_level or compiler version
"""

    prompt = f"""You are decompiling a GameCube (PowerPC) function for Pokemon Colosseum.
Convert the following PPC assembly into byte-matching C89 code.

CRITICAL RULES:
- C89 only: ALL declarations before statements in each block
- Use block scoping {{ }} when the asm loads the SAME global from r13 twice
- NEVER use float literals (0.0f) for sdata2 returns — use `extern f32 lbl_XXXXXXXX;`
- SDA_BASE = 0x80480820 (r13), SDA2_BASE = 0x804836A0 (r2)
- Signed 16-bit offset: if raw >= 0x8000, subtract 0x10000
- Leaf functions (nofralloc, no bl) should NOT have pragmas — use default O4
- Thunks (stwu/mflr/bl/epilogue) are just: `extern void target(); void fn() {{ target(); }}`
- `subf rD, rA, rB` means rD = rB - rA (reversed!)
- `clrrwi rD, rS, N` means rD = rS & ~((1<<N)-1)
- `extrwi rD, rS, n, b` extracts n bits starting at bit b
- Output ONLY the C function + needed externs. No asm, no #if blocks, no explanation.

{example}

FUNCTION: {wrapper['function']}
FILE: {wrapper['file']}
DETECTED PATTERN: {pattern}

ASSEMBLY ({wrapper['inc_file']}):
```
{inc_content}
```

SDA ADDRESS HINTS:
{sda_section}

SURROUNDING C CONTEXT:
```c
{context}
```

PRAGMAS ON WRAPPER: {', '.join(wrapper['pragmas']) if wrapper['pragmas'] else 'default (none)'}
NOTE: Wrapper pragmas are often WRONG for the C replacement. Most leaf functions match at default O4.
{retry_section}
Write the matching C89 function."""
    return prompt


def cmd_scan(args):
    """Scan and classify all wrappers."""
    ensure_dirs()
    wrappers = scan_wrappers()

    # Filter out already-completed
    progress = load_progress()
    completed_fns = set(progress["completed"])
    wrappers = [w for w in wrappers if w["function"] not in completed_fns]

    report_path = save_scan_report(wrappers, args.report)
    if args.update_queue:
        save_queue(wrappers)

    # Print summary
    by_tier = {}
    by_file = {}
    for w in wrappers:
        by_tier.setdefault(w["tier"], []).append(w)
        by_file.setdefault(w["file"], []).append(w)

    print(f"\n{'='*60}")
    print(f"DECOMP WORK QUEUE SCAN")
    print(f"{'='*60}")
    print(f"Total remaining wrappers: {len(wrappers)}")
    print(f"Report: {report_path.relative_to(ROOT)}")
    if args.update_queue:
        print(f"Queue updated: {QUEUE_FILE.relative_to(ROOT)}")
    else:
        print("Queue not updated. Pass --update-queue to rewrite work_queue.json.")
    print()
    print("By complexity tier:")
    for tier in [TIER_SIMPLE, TIER_MEDIUM, TIER_COMPLEX, TIER_HARD]:
        items = by_tier.get(tier, [])
        backends = ", ".join(TIER_BACKENDS.get(tier, []))
        print(f"  {tier:10s}: {len(items):4d} functions  -> backends: {backends}")

    print()
    print("Top 10 files by remaining wrappers:")
    sorted_files = sorted(by_file.items(), key=lambda x: -len(x[1]))
    for f, items in sorted_files[:10]:
        tiers = {}
        for w in items:
            tiers[w["tier"]] = tiers.get(w["tier"], 0) + 1
        tier_str = " ".join(f"{t}:{c}" for t, c in sorted(tiers.items()))
        print(f"  {len(items):4d}  {f}  ({tier_str})")

    print(f"\n{'='*60}")


def cmd_generate(args):
    """Generate N task prompts for the specified tier."""
    ensure_dirs()
    queue = load_queue()
    if not queue:
        print("No queue. Run --scan first.")
        return

    tier = args.tier or TIER_SIMPLE
    count = args.generate

    candidates = [w for w in queue if w["tier"] == tier and w["status"] == "pending"]
    candidates.sort(key=lambda w: w["asm_lines"])

    generated = 0
    for w in candidates[:count]:
        prompt = generate_prompt(w)
        if prompt:
            fn = w["function"]
            prompt_file = PROMPTS_DIR / f"{fn}.txt"
            prompt_file.write_text(prompt)
            print(f"Generated: {prompt_file.name} ({w['asm_lines']} lines, {w['tier']})")
            generated += 1

    print(f"\nGenerated {generated} prompts in {PROMPTS_DIR}")


def cmd_verify(args):
    """Verify a decompiled function by compiling and match testing."""
    fn_name = args.verify
    # Find the function in the queue
    queue = load_queue()
    wrapper = None
    for w in queue:
        if w["function"] == fn_name:
            wrapper = w
            break

    if not wrapper:
        print(f"Function {fn_name} not found in queue.")
        return

    c_file = wrapper["file"]

    # Compile
    print(f"Compiling {c_file}...")
    result = subprocess.run(
        ["python", "tools/compile_check.py", c_file],
        capture_output=True, text=True, cwd=str(ROOT)
    )
    if result.returncode != 0:
        print(f"COMPILE FAILED:\n{result.stdout}\n{result.stderr}")
        return False

    # Match test
    print(f"Match testing {fn_name}...")
    result = subprocess.run(
        ["python", "tools/match_test.py", fn_name],
        capture_output=True, text=True, cwd=str(ROOT)
    )
    output = result.stdout + result.stderr
    print(output)

    if "MATCHING" in output:
        # Update progress
        progress = load_progress()
        if fn_name not in progress["completed"]:
            progress["completed"].append(fn_name)
        save_progress(progress)
        release_lock(fn_name)
        print(f"\n*** {fn_name} VERIFIED 100% MATCH ***")
        return True
    else:
        print(f"\n{fn_name} not yet matching.")
        return False


def cmd_status(args):
    """Show overall progress."""
    ensure_dirs()
    progress = load_progress()
    queue = load_queue()

    total = len(queue) + len(progress["completed"])
    done = len(progress["completed"])
    remaining = len(queue)

    print(f"\n{'='*60}")
    print(f"DECOMP PROGRESS")
    print(f"{'='*60}")
    print(f"Completed:  {done}")
    print(f"Remaining:  {remaining}")
    print(f"Total:      {total}")
    if total > 0:
        print(f"Progress:   {done/total*100:.1f}%")

    if queue:
        by_tier = {}
        for w in queue:
            by_tier.setdefault(w["tier"], []).append(w)
        print("\nRemaining by tier:")
        for tier in [TIER_SIMPLE, TIER_MEDIUM, TIER_COMPLEX, TIER_HARD]:
            items = by_tier.get(tier, [])
            if items:
                print(f"  {tier:10s}: {len(items)}")

    # Show locks (in-progress)
    locks = list(LOCK_DIR.glob("*.lock"))
    if locks:
        print(f"\nIn progress ({len(locks)}):")
        for l in locks:
            print(f"  {l.stem}")

    print(f"{'='*60}")


def main():
    parser = argparse.ArgumentParser(description="Decomp Task Scheduler")
    parser.add_argument("--scan", action="store_true", help="Scan and classify all wrappers")
    parser.add_argument(
        "--report",
        help="Write --scan JSON report here (default: tools/decomp_work/reports/...)"
    )
    parser.add_argument(
        "--update-queue", action="store_true",
        help="With --scan, rewrite tools/decomp_work/work_queue.json"
    )
    parser.add_argument("--generate", type=int, metavar="N", help="Generate N task prompts")
    parser.add_argument("--tier", choices=["simple", "medium", "complex", "hard"],
                       help="Tier for --generate (default: simple)")
    parser.add_argument("--verify", metavar="FN", help="Verify a decompiled function")
    parser.add_argument("--status", action="store_true", help="Show progress")

    args = parser.parse_args()

    if args.scan:
        cmd_scan(args)
    elif args.generate:
        cmd_generate(args)
    elif args.verify:
        cmd_verify(args)
    elif args.status:
        cmd_status(args)
    else:
        parser.print_help()


if __name__ == "__main__":
    main()

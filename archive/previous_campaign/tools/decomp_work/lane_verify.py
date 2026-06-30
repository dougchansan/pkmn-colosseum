#!/usr/bin/env python3
"""Lane 2: Verification + Apply pipeline — picks up candidates, applies, tests"""
import json, os, re, subprocess, sys, time
from pathlib import Path

ROOT = Path(__file__).parent.parent.parent
sys.path.insert(0, str(ROOT / "tools"))

CANDIDATES = ROOT / "tools" / "decomp_work" / "candidates"
APPLIED = ROOT / "tools" / "decomp_work" / "applied"
REVIEW = ROOT / "tools" / "decomp_work" / "review"
PROGRESS = ROOT / "tools" / "decomp_work" / "progress.json"

def load_progress():
    if PROGRESS.exists():
        return json.loads(PROGRESS.read_text())
    return {"completed": [], "failed": [], "in_progress": []}

def save_progress(p):
    PROGRESS.write_text(json.dumps(p, indent=2))

def find_wrapper_block(lines, fn_name, start_hint):
    for offset in range(-5, 20):
        i = start_hint + offset
        if 0 <= i < len(lines) and lines[i].strip() == "#if 1":
            if i + 1 < len(lines) and fn_name in lines[i + 1]:
                # Find #endif
                depth = 0
                for j in range(i, min(len(lines), i + 100)):
                    if lines[j].strip().startswith("#if"): depth += 1
                    elif lines[j].strip() == "#endif":
                        depth -= 1
                        if depth == 0:
                            # Find pragma boundaries
                            ps = i
                            for k in range(i - 1, max(0, i - 8), -1):
                                s = lines[k].strip()
                                if s == "#pragma push": ps = k; break
                                if s == "#pragma pop" or not s.startswith("#pragma"): break
                                ps = k
                            pe = j
                            if j + 1 < len(lines) and lines[j + 1].strip() == "#pragma pop":
                                pe = j + 1
                            return (i, j, ps, pe)
    return None

def apply_and_test(candidate):
    fn = candidate["function"]
    c_file = ROOT / candidate["file"]
    code = candidate["code"]

    # Backup
    backup = c_file.read_text(encoding="utf-8", errors="replace")

    # Find wrapper in file
    lines = backup.split("\n")
    # Search for the function in the file
    hint = -1
    for i, l in enumerate(lines):
        if fn in l and "#if 1" in lines[max(0,i-1):i+1]:
            hint = i - 1
            break
        if l.strip() == "#if 1" and i + 1 < len(lines) and fn in lines[i+1]:
            hint = i
            break

    if hint < 0:
        return "skip", "function not found as #if 1 wrapper"

    block = find_wrapper_block(lines, fn, hint)
    if not block:
        return "skip", "could not find wrapper block"

    if_start, endif_line, pragma_start, pragma_end = block

    # Apply
    new_lines = lines[:pragma_start] + [code] + lines[pragma_end + 1:]
    c_file.write_text("\n".join(new_lines), encoding="utf-8")

    # Compile
    result = subprocess.run(
        ["python", "tools/compile_check.py", candidate["file"]],
        capture_output=True, text=True, cwd=str(ROOT), timeout=60)
    if result.returncode != 0 or "FAIL" in result.stdout:
        c_file.write_text(backup, encoding="utf-8")
        return "compile_error", result.stdout[:200]

    # Match test
    result = subprocess.run(
        ["python", "tools/match_test.py", fn],
        capture_output=True, text=True, cwd=str(ROOT), timeout=60)
    output = result.stdout + result.stderr

    if "MATCHING" in output:
        return "match", output
    else:
        m = re.search(r'(\d+\.\d+)%', output)
        pct = float(m.group(1)) if m else 0.0
        # Revert
        c_file.write_text(backup, encoding="utf-8")
        return f"partial_{pct:.0f}", output

def main():
    APPLIED.mkdir(parents=True, exist_ok=True)
    REVIEW.mkdir(parents=True, exist_ok=True)

    print(f"{'='*60}")
    print(f"LANE 2: VERIFY + APPLY")
    print(f"Watching: {CANDIDATES}")
    print(f"{'='*60}\n")

    matched = 0
    reviewed = 0
    rejected = 0

    while True:
        candidates = sorted(CANDIDATES.glob("*.json"))
        if not candidates:
            print("Waiting for candidates...", end="\r", flush=True)
            time.sleep(3)
            continue

        for cfile in candidates:
            try:
                candidate = json.loads(cfile.read_text(encoding="utf-8"))
            except:
                continue

            fn = candidate["function"]
            print(f"\nVerifying {fn} ({candidate.get('tier','?')})...", end=" ", flush=True)

            status, detail = apply_and_test(candidate)

            if status == "match":
                print(f"100% MATCH!")
                candidate["status"] = "matched"
                (APPLIED / cfile.name).write_text(json.dumps(candidate, indent=2), encoding="utf-8")
                cfile.unlink()
                progress = load_progress()
                if fn not in progress["completed"]:
                    progress["completed"].append(fn)
                save_progress(progress)
                matched += 1
            elif status.startswith("partial_"):
                pct = status.split("_")[1]
                print(f"{pct}% — sent to REVIEW")
                candidate["status"] = status
                candidate["detail"] = detail[:300]
                (REVIEW / cfile.name).write_text(json.dumps(candidate, indent=2), encoding="utf-8")
                cfile.unlink()
                reviewed += 1
            elif status == "skip":
                print(f"SKIP: {detail}")
                cfile.unlink()
            else:
                print(f"REJECT: {status}")
                cfile.unlink()
                rejected += 1

            print(f"  [matched={matched} review={reviewed} reject={rejected}]")

        time.sleep(2)

if __name__ == "__main__":
    main()

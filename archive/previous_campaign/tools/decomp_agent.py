#!/usr/bin/env python3
"""
Decomp Agent Runner

Sends decomp prompts to different backends (Ollama, OpenCode free models)
and verifies results. Coordinates with decomp_scheduler.py for task management.

Usage:
    python tools/decomp_agent.py --backend ollama --tier simple --count 5
    python tools/decomp_agent.py --backend ollama --function fn_XXXXXXXX
    python tools/decomp_agent.py --apply-and-verify fn_XXXXXXXX result.txt
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
WORK_DIR = ROOT / "tools" / "decomp_work"
QUEUE_FILE = WORK_DIR / "work_queue.json"
PROGRESS_FILE = WORK_DIR / "progress.json"
LOCK_DIR = WORK_DIR / "locks"
PROMPTS_DIR = WORK_DIR / "prompts"
RESULTS_DIR = WORK_DIR / "results"

OLLAMA_HOST = os.environ.get("DECOMP_GPU_HOST", "192.168.50.101")
OLLAMA_PORT = int(os.environ.get("DECOMP_OLLAMA_PORT", "11434"))
OLLAMA_MODEL = "codestral:22b"  # Best benchmark: 100% structural, 5.4s/fn
KIMI_URL = "https://api.moonshot.ai/v1/chat/completions"
KIMI_MODEL = "kimi-k2-turbo-preview"

# On macOS/Linux `codex` is on PATH; on Windows it's the .cmd shim. Override
# with CODEX_CMD if installed elsewhere.
CODEX_CMD = os.environ.get(
    "CODEX_CMD",
    "C:/Users/douglaswhittingham/AppData/Roaming/npm/codex.cmd"
    if os.name == "nt"
    else "codex",
)


def ensure_dirs():
    for d in [WORK_DIR, LOCK_DIR, PROMPTS_DIR, RESULTS_DIR]:
        d.mkdir(parents=True, exist_ok=True)


def load_queue():
    if QUEUE_FILE.exists():
        return json.loads(QUEUE_FILE.read_text())
    return []


def save_queue(queue):
    QUEUE_FILE.write_text(json.dumps(queue, indent=2))


def load_progress():
    if PROGRESS_FILE.exists():
        return json.loads(PROGRESS_FILE.read_text())
    return {"completed": [], "failed": [], "in_progress": []}


def save_progress(progress):
    PROGRESS_FILE.write_text(json.dumps(progress, indent=2))


def acquire_lock(fn_name):
    """Atomic-ish lock to prevent two agents from working the same function."""
    lock_file = LOCK_DIR / f"{fn_name}.lock"
    if lock_file.exists():
        # Check if stale (> 30 minutes)
        mtime = lock_file.stat().st_mtime
        if time.time() - mtime > 1800:
            lock_file.unlink()
        else:
            return False
    lock_file.write_text(json.dumps({
        "pid": os.getpid(),
        "time": time.time(),
        "host": os.environ.get("COMPUTERNAME", "unknown"),
    }))
    return True


def release_lock(fn_name):
    lock_file = LOCK_DIR / f"{fn_name}.lock"
    if lock_file.exists():
        lock_file.unlink()


def call_ollama(prompt, model=None):
    """Send prompt to Ollama on the GPU machine."""
    model = model or OLLAMA_MODEL

    url = f"http://{OLLAMA_HOST}:{OLLAMA_PORT}/api/generate"
    payload = json.dumps({
        "model": model,
        "prompt": prompt,
        "stream": False,
        "options": {
            "temperature": 0.1,
            "num_predict": 2048,
        }
    }).encode()

    req = urllib.request.Request(url, data=payload,
                                 headers={"Content-Type": "application/json"})
    try:
        with urllib.request.urlopen(req, timeout=120) as resp:
            result = json.loads(resp.read())
            return result.get("response", "")
    except Exception as e:
        print(f"Ollama error: {e}")
        return None


def call_kimi(prompt, model=None):
    """Send prompt to Moonshot Kimi using an environment-provided API key."""
    model = model or os.environ.get("MOONSHOT_MODEL", KIMI_MODEL)
    api_key = (
        os.environ.get("MOONSHOT_API_KEY")
        or os.environ.get("KIMI_API_KEY")
    )
    if not api_key:
        print("Kimi error: set MOONSHOT_API_KEY or KIMI_API_KEY in the environment")
        return None

    payload = json.dumps({
        "model": model,
        "messages": [{"role": "user", "content": prompt}],
        "temperature": 0.1,
        "max_tokens": 2048,
    }).encode()
    req = urllib.request.Request(
        KIMI_URL,
        data=payload,
        headers={
            "Content-Type": "application/json",
            "Authorization": f"Bearer {api_key}",
        },
    )

    try:
        with urllib.request.urlopen(req, timeout=120) as resp:
            result = json.loads(resp.read())
            return result["choices"][0]["message"]["content"]
    except Exception as e:
        print(f"Kimi error: {e}")
        return None


def call_codex(prompt):
    """Send prompt to Codex CLI for processing."""
    import tempfile
    output_file = RESULTS_DIR / "_codex_output.txt"

    try:
        # Write prompt to temp file to avoid encoding issues
        prompt_file = RESULTS_DIR / "_codex_prompt.txt"
        prompt_file.write_text(prompt, encoding="utf-8")

        result = subprocess.run(
            [CODEX_CMD, "exec",
             "--ephemeral",
             "-o", str(output_file),
             f"Read the file {prompt_file} and follow the instructions in it. Output ONLY the C89 function code."],
            capture_output=True, text=True, cwd=str(ROOT),
            timeout=180,
            env={**os.environ, "PYTHONIOENCODING": "utf-8"}
        )
        if output_file.exists():
            response = output_file.read_text(encoding="utf-8", errors="replace")
            output_file.unlink()
            return response
        return result.stdout if result.stdout else result.stderr
    except subprocess.TimeoutExpired:
        print("Codex timed out")
        return None
    except Exception as e:
        print(f"Codex error: {e}")
        return None


def extract_c_code(response):
    """Extract C code from model response, stripping markdown fences."""
    if not response:
        return None

    # Try to find code in ```c ... ``` blocks
    m = re.search(r'```c\s*\n(.*?)```', response, re.DOTALL)
    if m:
        return m.group(1).strip()

    # Try ``` ... ```
    m = re.search(r'```\s*\n(.*?)```', response, re.DOTALL)
    if m:
        return m.group(1).strip()

    # Return raw if it looks like C code
    if "void " in response or "return " in response or "extern " in response:
        # Strip any leading explanation text
        lines = response.split("\n")
        code_start = 0
        for i, line in enumerate(lines):
            if line.strip().startswith(("extern ", "void ", "u32 ", "s32 ",
                                        "f32 ", "u8 ", "u16 ", "#pragma ",
                                        "static ")):
                code_start = i
                break
        return "\n".join(lines[code_start:]).strip()

    return None


def normalize_c_code(c_code, fn_name):
    """Normalize model output to project conventions."""
    # Type replacements (stdint -> project types)
    replacements = [
        ("uint32_t", "u32"), ("uint16_t", "u16"), ("uint8_t", "u8"),
        ("int32_t", "s32"), ("int16_t", "s16"), ("int8_t", "s8"),
        ("uintptr_t", "u32"), ("intptr_t", "s32"),
        ("size_t", "u32"), ("bool", "u32"),
        ("NULL", "(void*)0"),  # some models use NULL without include
    ]
    for old, new in replacements:
        c_code = c_code.replace(old, new)

    # Remove #include lines (project headers are handled elsewhere)
    lines = c_code.split("\n")
    lines = [l for l in lines if not l.strip().startswith("#include")]

    # Remove __attribute__ annotations
    c_code = "\n".join(lines)
    c_code = re.sub(r'__attribute__\s*\([^)]*\)', '', c_code)

    # Fix NULL → 0 for pointer contexts (CW prefers literal 0)
    # But keep (void*)0 → just use 0
    c_code = c_code.replace("(void*)0", "0")

    return c_code.strip()


def find_wrapper_block(lines, fn_name, start_hint):
    """Find the full #if 1 ... #endif block for a function.
    Returns (if_start, endif_line, pragma_start, pragma_end) or None."""

    # Search around the hint for the #if 1 line
    search_start = max(0, start_hint - 5)
    search_end = min(len(lines), start_hint + 5)

    if_start = None
    for i in range(search_start, search_end):
        if lines[i].strip() == "#if 1":
            # Verify next line contains the function name
            if i + 1 < len(lines) and fn_name in lines[i + 1]:
                if_start = i
                break

    if if_start is None:
        # Broader search: find #if 1 followed by asm void fn_name
        for i in range(max(0, start_hint - 20), min(len(lines), start_hint + 20)):
            if lines[i].strip() == "#if 1":
                if i + 1 < len(lines) and fn_name in lines[i + 1]:
                    if_start = i
                    break

    if if_start is None:
        return None

    # Find matching #endif (search up to 100 lines for long #else blocks)
    endif_line = None
    depth = 0
    for i in range(if_start, min(len(lines), if_start + 100)):
        stripped = lines[i].strip()
        if stripped.startswith("#if"):
            depth += 1
        elif stripped == "#endif":
            depth -= 1
            if depth == 0:
                endif_line = i
                break

    if endif_line is None:
        return None

    # Find pragma push/pop boundaries
    pragma_start = if_start
    for i in range(if_start - 1, max(0, if_start - 8), -1):
        stripped = lines[i].strip()
        if stripped == "#pragma push":
            pragma_start = i
            break
        if stripped == "#pragma pop" or not stripped.startswith("#pragma"):
            # Hit a pop or non-pragma line, stop
            break
        # Accumulate pragma lines (optimization_level, optimizewithasm)
        pragma_start = i

    pragma_end = endif_line
    if endif_line + 1 < len(lines) and lines[endif_line + 1].strip() == "#pragma pop":
        pragma_end = endif_line + 1

    return (if_start, endif_line, pragma_start, pragma_end)


def collect_existing_externs(lines, block_start):
    """Collect extern declarations in the 10 lines before the block."""
    externs = []
    for i in range(max(0, block_start - 10), block_start):
        stripped = lines[i].strip()
        if stripped.startswith("extern "):
            externs.append(stripped)
    return externs


def apply_result(fn_name, c_code, wrapper_info):
    """Apply the decompiled C code to the source file, replacing the asm wrapper."""
    c_file = ROOT / wrapper_info["file"]
    content = c_file.read_text(encoding="utf-8", errors="replace")
    lines = content.split("\n")
    ln = wrapper_info["line_number"] - 1  # 0-indexed

    # Normalize model output
    c_code = normalize_c_code(c_code, fn_name)

    # Find the wrapper block
    block = find_wrapper_block(lines, fn_name, ln)
    if block is None:
        print(f"Could not find wrapper block for {fn_name} near line {ln+1}")
        return False

    if_start, endif_line, pragma_start, pragma_end = block

    # Collect existing externs (keep them, don't duplicate)
    existing_externs = collect_existing_externs(lines, pragma_start)

    # Remove extern lines from c_code if they already exist in the file
    c_lines = c_code.split("\n")
    filtered_lines = []
    for cl in c_lines:
        stripped = cl.strip()
        if stripped.startswith("extern ") and stripped.rstrip(";") + ";" in [e.rstrip(";") + ";" for e in existing_externs]:
            continue  # skip duplicate extern
        filtered_lines.append(cl)
    c_code = "\n".join(filtered_lines)

    # Build replacement
    new_lines = lines[:pragma_start] + [c_code] + lines[pragma_end + 1:]
    c_file.write_text("\n".join(new_lines), encoding="utf-8")
    return True


def verify_function(fn_name, c_file_path):
    """Compile and match test a function."""
    # Compile
    result = subprocess.run(
        ["python", "tools/compile_check.py", c_file_path],
        capture_output=True, text=True, cwd=str(ROOT)
    )
    if result.returncode != 0 or "FAIL" in result.stdout:
        return "compile_error", result.stdout

    # Match test
    result = subprocess.run(
        ["python", "tools/match_test.py", fn_name],
        capture_output=True, text=True, cwd=str(ROOT)
    )
    output = result.stdout + result.stderr

    if "MATCHING" in output:
        return "match", output
    else:
        # Extract match percentage
        m = re.search(r'(\d+\.\d+)%', output)
        pct = float(m.group(1)) if m else 0.0
        return f"partial_{pct:.0f}", output


def get_instruction_diff(fn_name, obj_path):
    """Get human-readable instruction diff via objdiff."""
    diff_file = RESULTS_DIR / f"{fn_name}_diff.json"
    try:
        subprocess.run([
            str(ROOT / "tools" / ("objdiff-cli.exe" if os.name == "nt" else "objdiff-cli")), "diff",
            "-1", str(ROOT / "build" / "GC6E01" / "obj" / "auto_01_800055E0_text.o"),
            "-2", str(ROOT / "build" / "GC6E01" / "base" / obj_path),
            "-o", str(diff_file), "--format", "json", fn_name
        ], capture_output=True, timeout=30, cwd=str(ROOT))

        data = json.loads(diff_file.read_text())
        lines = []
        for side_name in ["left", "right"]:
            side = data.get(side_name, {})
            for sym in side.get("symbols", []):
                if sym.get("name") == fn_name:
                    label = "TARGET" if side_name == "left" else "YOURS"
                    lines.append(f"=== {label} ===")
                    for i, e in enumerate(sym.get("instructions", [])):
                        inst = e.get("instruction", {})
                        dk = e.get("diff_kind", "")
                        fmt = inst.get("formatted", "")
                        marker = "!!" if dk and dk != "NONE" else "  "
                        lines.append(f"  {i:3d} {marker} {fmt}")
                    break
        return "\n".join(lines)
    except Exception as e:
        return f"(diff unavailable: {e})"


def process_function(fn_name, backend="ollama", max_retries=3):
    """Process a single function with retry-on-failure and diff feedback."""
    ensure_dirs()
    queue = load_queue()

    wrapper = None
    for w in queue:
        if w["function"] == fn_name:
            wrapper = w
            break

    if not wrapper:
        print(f"{fn_name} not found in queue.")
        return False

    if not acquire_lock(fn_name):
        print(f"{fn_name} is locked by another agent.")
        return False

    print(f"\n{'='*50}")
    print(f"Processing: {fn_name} ({wrapper['asm_lines']} lines, {wrapper['tier']})")
    print(f"Backend: {backend}, max retries: {max_retries}")
    print(f"{'='*50}")

    # Back up original file ONCE
    c_file = ROOT / wrapper["file"]
    backup = RESULTS_DIR / f"{fn_name}_backup.c"
    backup.write_text(c_file.read_text(encoding="utf-8", errors="replace"))

    from decomp_scheduler import generate_prompt

    retry_diff = None
    best_status = "none"

    for attempt in range(1, max_retries + 1):
        print(f"\n--- Attempt {attempt}/{max_retries} ---")

        # Restore from backup before each attempt
        c_file.write_text(backup.read_text())

        # Generate prompt (with diff feedback on retries)
        prompt = generate_prompt(wrapper, retry_diff=retry_diff)
        if not prompt:
            print(f"Could not generate prompt for {fn_name}")
            break

        (PROMPTS_DIR / f"{fn_name}_attempt{attempt}.txt").write_text(prompt, encoding="utf-8")

        # Call model
        print(f"Calling {backend}...")
        if backend == "ollama":
            response = call_ollama(prompt)
        elif backend == "kimi":
            response = call_kimi(prompt)
        elif backend == "codex":
            response = call_codex(prompt)
        else:
            print(f"Unknown backend: {backend}")
            break

        if not response:
            print(f"No response from {backend}")
            continue

        (RESULTS_DIR / f"{fn_name}_response_{attempt}.txt").write_text(response, encoding="utf-8")

        # Extract C code
        c_code = extract_c_code(response)
        if not c_code:
            print(f"Could not extract C code from response")
            continue

        (RESULTS_DIR / f"{fn_name}_code_{attempt}.c").write_text(c_code, encoding="utf-8")
        print(f"Extracted C code ({len(c_code)} chars)")

        # Apply
        c_file.write_text(backup.read_text())  # fresh restore
        if not apply_result(fn_name, c_code, wrapper):
            print("Failed to apply result")
            continue

        # Verify
        print("Verifying...")
        status, output = verify_function(fn_name, wrapper["file"])

        if status == "match":
            print(f"\n*** {fn_name} MATCHED 100% on attempt {attempt}! ***")
            progress = load_progress()
            if fn_name not in progress["completed"]:
                progress["completed"].append(fn_name)
            save_progress(progress)
            release_lock(fn_name)
            return True

        print(f"Attempt {attempt}: {status}")
        best_status = status

        # Get diff for retry feedback
        obj_rel = wrapper["file"].replace("src/", "").replace(".c", ".o")
        retry_diff = get_instruction_diff(fn_name, obj_rel)
        if retry_diff:
            (RESULTS_DIR / f"{fn_name}_diff_{attempt}.txt").write_text(retry_diff, encoding="utf-8")
            print(f"Got instruction diff for retry feedback")

    # All retries exhausted
    print(f"\n{fn_name}: best result was {best_status} after {max_retries} attempts")
    print("Reverting to backup...")
    c_file.write_text(backup.read_text())
    release_lock(fn_name)

    progress = load_progress()
    progress["failed"].append({
        "function": fn_name,
        "status": best_status,
        "backend": backend,
        "time": time.time(),
    })
    save_progress(progress)
    return False


def cmd_run(args):
    """Run decomp agent on functions."""
    ensure_dirs()
    queue = load_queue()
    if not queue:
        print("No queue. Run decomp_scheduler.py --scan first.")
        return

    sys.path.insert(0, str(ROOT / "tools"))

    if args.function:
        process_function(args.function, args.backend)
    else:
        tier = args.tier or "simple"
        count = args.count or 5
        candidates = [w for w in queue
                     if w["tier"] == tier and w["status"] == "pending"]
        candidates.sort(key=lambda w: w["asm_lines"])

        completed_fns = set(load_progress()["completed"])
        candidates = [c for c in candidates if c["function"] not in completed_fns]

        successes = 0
        failures = 0
        for w in candidates[:count]:
            if process_function(w["function"], args.backend):
                successes += 1
            else:
                failures += 1

        print(f"\n{'='*50}")
        print(f"Batch complete: {successes} matched, {failures} failed")
        print(f"{'='*50}")


def main():
    parser = argparse.ArgumentParser(description="Decomp Agent Runner")
    parser.add_argument("--backend", choices=["ollama", "kimi", "codex", "opencode"],
                       default="ollama", help="Model backend")
    parser.add_argument("--function", metavar="FN", help="Process a specific function")
    parser.add_argument("--tier", choices=["simple", "medium", "complex", "hard"],
                       help="Process functions of this tier")
    parser.add_argument("--count", type=int, default=5, help="Number of functions to process")

    args = parser.parse_args()
    cmd_run(args)


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
"""make_expert_suite.py — rewrite a compact CW benchmark suite with the EXPERT prompt.

Reads test_suite_cw_focus_compact.json and produces test_suite_cw_focus_expert.json
with a richer per-test prompt that includes:
  - Full CW_QUIRKS.md cheatsheet
  - Full few_shot_examples.md
  - REFERENCE_PROJECTS.md (other GCN decomp repos to learn from)
  - 2-3 PEER FUNCTIONS extracted from the target .c file (already-decomp'd siblings)
  - All EXISTING EXTERNS in the target .c file
  - Wider CONTEXT (40 lines before/after the target function instead of ~10)
  - Original ASM and SDA hints

Usage:
    python tools/decomp_work/benchmark/make_expert_suite.py \
        --in  tools/decomp_work/benchmark/test_suite_cw_focus_compact.json \
        --out tools/decomp_work/benchmark/test_suite_cw_focus_expert.json
"""
from __future__ import annotations

import argparse
import json
import re
from pathlib import Path

REPO = Path(__file__).resolve().parents[3]
DECOMP_WORK = REPO / "tools" / "decomp_work"
CW_QUIRKS = (DECOMP_WORK / "CW_QUIRKS.md").read_text(encoding="utf-8")
FEW_SHOT = (DECOMP_WORK / "few_shot_examples.md").read_text(encoding="utf-8")
REF_PROJECTS = (DECOMP_WORK / "REFERENCE_PROJECTS.md").read_text(encoding="utf-8")


def read_c_file(path: str) -> str:
    p = REPO / path
    if not p.exists():
        return ""
    return p.read_text(encoding="latin-1")


def extract_externs(c_text: str, limit: int = 60) -> list[str]:
    """Pull `extern ...;` declarations from the file."""
    out = []
    for line in c_text.splitlines():
        s = line.strip()
        if s.startswith("extern ") and s.endswith(";"):
            out.append(s)
            if len(out) >= limit:
                break
    return out


def extract_existing_else_body(c_text: str, target_fn: str) -> str | None:
    """If the file has `#if 1\\nasm void <fn>\\n...\\n#else\\n<C body>\\n#endif`, return the C body.

    This is the existing partial decomp that already compiles. Cloud models do FAR better when
    given this as a starting point + asm diff target than asked to write from scratch.
    """
    pattern = re.compile(
        r"#if\s+[01]\s*\nasm void " + re.escape(target_fn) + r"\(void\)\s*\{[^}]*\}\s*\n"
        r"#else\s*\n(.*?)\n#endif",
        re.S,
    )
    m = pattern.search(c_text)
    if m:
        body = m.group(1).strip()
        # Skip stub-only bodies (TODO comments, single-line returns)
        if len(body) < 60 or "/* TODO" in body[:80]:
            return None
        return body
    return None


def extract_peer_functions(c_text: str, target_fn: str, max_count: int = 3) -> list[str]:
    """Find up to `max_count` already-decompiled fn_XXXX siblings in the .c file
    (functions that have a real C body, not just `asm void ... { #include ... }`)."""
    fn_pattern = re.compile(
        r"(?m)^(?:[ \t]*(?:[A-Za-z_][\w *]+?[ \t*])?fn_[0-9A-Fa-f]{8}[ \t]*\([^;]*\)[ \t]*\{)"
    )
    matches = list(fn_pattern.finditer(c_text))
    if not matches:
        return []
    target_idx = -1
    for i, m in enumerate(matches):
        if target_fn in m.group(0):
            target_idx = i
            break
    candidates: list[str] = []
    indices = []
    if target_idx >= 0:
        for offset in (1, -1, 2, -2, 3, -3, 4, -4):
            j = target_idx + offset
            if 0 <= j < len(matches) and j != target_idx:
                indices.append(j)
    else:
        indices = list(range(min(len(matches), max_count + 2)))

    for j in indices:
        if len(candidates) >= max_count:
            break
        start = matches[j].start()
        # Find end of this function: balanced braces from the opening {
        depth = 0
        end = start
        in_string = False
        in_char = False
        in_comment = False
        for k in range(start, min(start + 6000, len(c_text))):
            ch = c_text[k]
            if in_comment:
                if ch == "/" and c_text[k - 1] == "*":
                    in_comment = False
                continue
            if in_string:
                if ch == "\\":
                    continue
                if ch == '"':
                    in_string = False
                continue
            if in_char:
                if ch == "\\":
                    continue
                if ch == "'":
                    in_char = False
                continue
            if ch == "/" and k + 1 < len(c_text) and c_text[k + 1] == "*":
                in_comment = True
                continue
            if ch == '"':
                in_string = True
            elif ch == "'":
                in_char = True
            elif ch == "{":
                depth += 1
            elif ch == "}":
                depth -= 1
                if depth == 0:
                    end = k + 1
                    break
        body = c_text[start:end]
        # Skip asm-stub or one-liner thunks shorter than 60 chars; skip oversized funcs > 1500
        if 60 < len(body) < 1500 and "#include" not in body and "asm " not in body[:40]:
            candidates.append(body)
    return candidates


def extract_wider_context(c_text: str, target_fn: str, lines_before: int = 50, lines_after: int = 30) -> str:
    """Return a wider source-file window around the target function."""
    lines = c_text.splitlines()
    target_idx = None
    for i, line in enumerate(lines):
        if target_fn in line and ("fn_" + target_fn[3:]) in line:
            target_idx = i
            break
    if target_idx is None:
        # try just the bare name
        for i, line in enumerate(lines):
            if target_fn in line:
                target_idx = i
                break
    if target_idx is None:
        return ""
    start = max(0, target_idx - lines_before)
    end = min(len(lines), target_idx + lines_after)
    return "\n".join(lines[start:end])


def build_expert_prompt(test: dict) -> str:
    fn = test["function"]
    asm = test["asm"]
    file_path = test["file"]
    c_text = read_c_file(file_path)
    externs = extract_externs(c_text)
    peers = extract_peer_functions(c_text, fn, max_count=2)
    existing_partial = extract_existing_else_body(c_text, fn)
    wider_ctx = extract_wider_context(c_text, fn, lines_before=40, lines_after=20)
    sda_hints = ""
    m = re.search(r"SDA ADDRESS HINTS:\n(.*?)\n\nCONTEXT", test["prompt"], re.DOTALL)
    if m:
        sda_hints = m.group(1).strip()

    parts = []
    parts.append(
        f"# Pokemon Colosseum decompilation — match `{fn}` in `{file_path}` byte-for-byte\n"
        "\n"
        "You are decompiling a GameCube (PowerPC) function compiled with **Metrowerks CodeWarrior 1.3**\n"
        "(flags `-O4,p -fp hard`). Your output, when compiled with that exact toolchain, must produce\n"
        "byte-identical PowerPC machine code to the target `.inc`. Floor: ≥90% match — but aim for 100%.\n"
        "\n"
        "## Output format (STRICT)\n"
        "Respond with ONE C code block (` ```c ... ``` `) containing the full replacement function, any\n"
        "necessary `extern` declarations, and any required `#pragma push / pragma ... / #pragma pop`\n"
        "wrapper. Do NOT output assembly, do NOT output #if/#else blocks, do NOT explain. Just the\n"
        "compilable C body.\n"
    )
    parts.append("\n## CW 1.3 cheatsheet (mandatory rules)\n\n")
    parts.append(CW_QUIRKS)
    parts.append("\n\n## Verified 100%-match patterns from this codebase\n\n")
    parts.append(FEW_SHOT)
    parts.append("\n\n## Other GCN decomp projects you can imitate\n\n")
    parts.append(REF_PROJECTS)
    parts.append("\n\n## Peer functions already decompiled in this same `.c` file\n")
    parts.append("(These are sibling functions in the SAME translation unit — match their style and reuse their externs.)\n\n")
    if peers:
        for i, body in enumerate(peers, 1):
            parts.append(f"### Peer example {i}\n```c\n{body}\n```\n\n")
    else:
        parts.append("(No fully-decompiled peers found in this file yet.)\n\n")
    parts.append("\n## Existing extern declarations in this file (LOCKED SIGNATURES)\n")
    parts.append(
        "**These signatures are LOCKED. CodeWarrior will refuse to compile if you redeclare any of these\n"
        "extern symbols with a different parameter list or return type. If the asm shows a function being\n"
        "called with arguments that DON'T match the locked signature, use the CW cast-fnptr idiom — DO\n"
        "NOT add a new extern. Examples:**\n\n"
        "```c\n"
        "// File already has: extern void fn_800D61E4(void);\n"
        "// But asm calls it with 2 args. Cast the fnptr at the CALL SITE:\n"
        "((void (*)(s32, s32))fn_800D61E4)(0, 0);\n"
        "((void (*)(s32, s32))fn_800D61E4)(*(s16*)(arg + 0x54), *(s16*)(arg + 0x56));\n"
        "\n"
        "// File already has: extern u32 lbl_8047B898;  (declared u32 but it holds a float)\n"
        "// Read it as float without redeclaring:\n"
        "*(f32*)&lbl_8047B898\n"
        "```\n\n"
        "Existing externs:\n```c\n"
    )
    parts.append("\n".join(externs[:80]))
    parts.append("\n```\n\n")
    parts.append(f"## Target function: `{fn}`\n\n")
    parts.append("### Assembly (target)\n```\n")
    parts.append(asm.strip())
    parts.append("\n```\n\n")
    if sda_hints:
        parts.append("### SDA address hints\n")
        parts.append(f"```\n{sda_hints}\n```\n\n")
    parts.append("### Wider source context (~70 lines around the target)\n```c\n")
    parts.append(wider_ctx)
    parts.append("\n```\n\n")
    if existing_partial:
        parts.append(
            "### EXISTING PARTIAL DECOMP — your starting point\n"
            "**This C body is already in the file's `#else` block. It compiles cleanly under CW 1.3 but\n"
            "currently matches 0% (or some non-100%). Your job is to REVISE it so the resulting CW\n"
            "codegen byte-matches the target asm above. Keep the structure that already works; modify\n"
            "ONLY what's needed to fix the asm diff. Common levers: pragma combos (push/peephole off,\n"
            "scheduling on, fp_contract on), control-flow shape (if vs switch, branch inversion),\n"
            "type casts (s16/s32/u32/u8) at use sites, block scoping for SDA reloads, cast-fnptr at\n"
            "call sites with mismatched extern signatures.**\n\n"
            "```c\n"
        )
        parts.append(existing_partial)
        parts.append("\n```\n\n")
    parts.append(
        "## Now write the matching C\n"
        f"Output ONLY the single ```c code block``` for `{fn}`.\n"
        "\n"
        "Hard rules (violating any of these = guaranteed compile fail or 0% match):\n"
        "1. Use ONLY the existing extern signatures above. Never add a new extern for a symbol that's\n"
        "   already declared. If the call signature differs, use cast-to-fnptr `((void (*)(args))fn_X)(a)`.\n"
        "2. C89 only — declare ALL locals at the top of each `{ }` block before any statements.\n"
        "3. Wrap the function body in `#pragma push / #pragma peephole off / ... / #pragma pop` (or\n"
        "   add `#pragma scheduling on` if the asm shows reordered loads/stores characteristic of CW\n"
        "   instruction scheduling).\n"
        "4. For SDA float reads where the extern is u32, use `*(f32*)&lbl_X` — do NOT add a new f32\n"
        "   extern.\n"
        "5. For CTR loops: inline the count expression into the `for(...)` init, never via a local.\n"
        "6. Match the target's switch shape: include `case 0:` AND `default:` together if the asm\n"
        "   shows the dead `cmpwi r0, 0; b @default` prologue.\n"
        "7. No #if/#else markers, no commentary, no asm — just the C body wrapped in one ```c block```.\n"
    )
    return "".join(parts)


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--in", dest="src", required=True)
    ap.add_argument("--out", dest="dst", required=True)
    args = ap.parse_args()

    suite = json.loads(Path(args.src).read_text(encoding="utf-8"))
    out = []
    for test in suite:
        new = dict(test)
        new["prompt"] = build_expert_prompt(test)
        out.append(new)
    Path(args.dst).write_text(json.dumps(out, indent=2), encoding="utf-8")
    sizes = [len(t["prompt"]) for t in out]
    print(f"wrote {args.dst}: {len(out)} tests, prompt sizes min={min(sizes)} avg={sum(sizes)//len(sizes)} max={max(sizes)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

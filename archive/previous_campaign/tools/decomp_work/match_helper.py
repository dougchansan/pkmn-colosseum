#!/usr/bin/env python3
"""match_helper.py — Analyze a .inc asm file and emit a C skeleton + pragma suggestions.

Usage:
    python3 tools/decomp_work/match_helper.py src/game/gs_field_world_fn_80115C48.inc

Outputs a suggested scaffold and detected patterns. Meant as a seed for any model
(or human) to flesh out the byte-match C. Never the final code — always verify via
compile_check.py + match_scan_file.py.
"""
from __future__ import annotations

import io
import re
import sys
from pathlib import Path
from dataclasses import dataclass, field

# Force UTF-8 stdout on Windows (default cp1252 chokes on → ≥ etc.)
if sys.stdout.encoding and sys.stdout.encoding.lower() != "utf-8":
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")


@dataclass
class Analysis:
    path: Path
    lines: list[str]
    size_bytes: int = 0
    # detected flags
    has_jumptable: bool = False
    has_cntlzw: bool = False
    has_divw: bool = False
    has_frsqrte: bool = False
    has_mtspr_gqr: bool = False
    has_fmadds: bool = False
    has_fnmsubs: bool = False
    has_fcmpo_cror: bool = False
    has_empty_else_ble: bool = False
    has_mtctr_bdnz: bool = False
    has_stmw: bool = False
    stmw_reg: int | None = None
    has_psq: bool = False
    has_int_to_float_magic: bool = False
    has_fctiwz: bool = False
    has_volatile_repeat_load: bool = False
    has_signed_byte_extsb: bool = False
    has_update_form: bool = False  # lwzu/stbu/stfsu

    calls: list[str] = field(default_factory=list)
    sda_symbols: list[str] = field(default_factory=list)
    rodata_symbols: list[str] = field(default_factory=list)
    jumptable_symbols: list[str] = field(default_factory=list)
    stack_frame: int | None = None
    callee_saved_gprs: list[int] = field(default_factory=list)
    callee_saved_fprs: list[int] = field(default_factory=list)

    skip_reasons: list[str] = field(default_factory=list)
    pragma_suggestions: list[str] = field(default_factory=list)
    pattern_hints: list[str] = field(default_factory=list)


SKIP_PATTERNS = {
    "bctr": "jump table (lwzx+bctr) — matching requires exact jumptable decl + case body order",
    "cntlzw": "count-leading-zeros bit trick — CW 1.3 doesn't expose as intrinsic",
    "divw": "signed 32-bit divide — hard to force from C without helper",
    "frsqrte": "reciprocal square root + Newton-Raphson — compiler-specific",
    "mtspr GQR": "paired-single quantization register — inline asm only",
}


def analyze(path: Path) -> Analysis:
    text = path.read_text(encoding="latin-1")
    lines = [ln.strip() for ln in text.splitlines() if ln.strip()]
    a = Analysis(path=path, lines=lines, size_bytes=sum(1 for ln in lines if not ln.startswith(("@", "#", ";"))) * 4)

    body = "\n".join(lines)

    # skip markers
    for pat, reason in SKIP_PATTERNS.items():
        if pat in body:
            a.skip_reasons.append(f"{pat}: {reason}")
    if "bctr" in body:
        a.has_jumptable = True
    if "cntlzw" in body:
        a.has_cntlzw = True
    if "divw" in body:
        a.has_divw = True
    if "frsqrte" in body:
        a.has_frsqrte = True
    if "mtspr GQR" in body:
        a.has_mtspr_gqr = True

    # codegen patterns
    if "fmadds" in body:
        a.has_fmadds = True
        a.pragma_suggestions.append("#pragma fp_contract on  // required for fmadds fusion")
    if "fnmsubs" in body:
        a.has_fnmsubs = True
        a.pragma_suggestions.append("#pragma fp_contract on  // required for fnmsubs: `b - a*c`")
    if "cror eq, gt, eq" in body or "cror eq, lt, eq" in body:
        a.has_fcmpo_cror = True
        a.pattern_hints.append(
            "cror eq,gt,eq + bne = `>=` only (NOT `>= || ==`); "
            "cror eq,lt,eq + bne = `<=` only (NOT `<= || ==`)"
        )
    if re.search(r"^\s*ble\s+@?L_\w+\s*$\n\s*b\s+@?L_\w+", body, re.M):
        a.has_empty_else_ble = True
        a.pattern_hints.append(
            "`ble/b` empty-else pair detected. Use `goto` to force shape:\n"
            "  if (A > X) goto SKIP;\n"
            "  A = -A;\n"
            "  SKIP:;"
        )
    if re.search(r"mtctr\s+\w+", body) and "bdnz" in body:
        a.has_mtctr_bdnz = True
        a.pattern_hints.append(
            "mtctr/bdnz detected. INLINE count load into for-init:\n"
            "  for (i = *(u32*)lbl_COUNT; i != 0; i--) { ... }\n"
            "Do NOT hoist count into a separate local with outer if-guard."
        )
    m = re.search(r"stmw\s+r(\d+)", body)
    if m:
        a.has_stmw = True
        a.stmw_reg = int(m.group(1))
        if a.stmw_reg <= 26:
            a.skip_reasons.append(
                f"stmw r{a.stmw_reg} saves 6+ callee GPRs — reg-pressure patterns are fragile"
            )
    if "psq_st" in body or "psq_l" in body:
        a.has_psq = True
    if re.search(r"xoris\s+\w+,\s*\w+,\s*0x8000", body):
        a.has_int_to_float_magic = True
        a.pattern_hints.append(
            "int→float magic (`xoris 0x8000` + lfd + fsubs) detected.\n"
            "Express as `(f32)(s32)val`. If target uses `lbl_XXXX@sda21` for the magic\n"
            "and yours hits `@NNNN` internal pool, it's toolchain-locked at ~93-97%."
        )
    if "fctiwz" in body:
        a.has_fctiwz = True
        a.pattern_hints.append(
            "fctiwz + stfd + lwz + sth pattern = `*(s16*)dst = (s32)float_expr;`\n"
            "DO NOT add outer `(s16)` cast — adds extra `extsh`."
        )
    if body.count("lbz r") >= 3 and re.search(r"lbz\s+\w+,\s*\w+\(\w+\).*?lbz\s+\w+,\s*\w+\(\w+\)", body, re.S):
        # heuristic: repeated byte load from same base
        a.has_volatile_repeat_load = True
        a.pattern_hints.append(
            "Repeated byte reads of same offset — may need `*(volatile u8*)(src + N)`\n"
            "to prevent CSE collapsing into one load."
        )
    if "extsb" in body:
        a.has_signed_byte_extsb = True
    if re.search(r"\b(lwzu|stbu|stwu r[^1]|stfsu)\b", body):
        a.has_update_form = True
        a.pattern_hints.append("lwzu/stbu/stfsu update-form load/store — hard to reproduce from C")

    # call extraction
    for m in re.finditer(r"bl\s+(fn_[0-9A-Fa-f]+)", body):
        if m.group(1) not in a.calls:
            a.calls.append(m.group(1))

    # SDA symbols
    for m in re.finditer(r"\b(lbl_[0-9A-Fa-f]+)\(r13\)", body):
        if m.group(1) not in a.sda_symbols:
            a.sda_symbols.append(m.group(1))
    for m in re.finditer(r"\b(lbl_[0-9A-Fa-f]+)\(r2\)", body):
        if m.group(1) not in a.sda_symbols:
            a.sda_symbols.append(m.group(1))
    # rodata / absolute
    for m in re.finditer(r"\b(lbl_[0-9A-Fa-f]+)@ha", body):
        if m.group(1) not in a.rodata_symbols:
            a.rodata_symbols.append(m.group(1))
    for m in re.finditer(r"\b(jumptable_[0-9A-Fa-f]+)", body):
        if m.group(1) not in a.jumptable_symbols:
            a.jumptable_symbols.append(m.group(1))

    # stack frame
    m = re.search(r"stwu\s+r1,\s*-0x([0-9A-Fa-f]+)\(r1\)", body)
    if m:
        a.stack_frame = int(m.group(1), 16)
    # callee saved via stw rN, offset(r1)
    for m in re.finditer(r"stw\s+r(\d+),\s*0x[0-9A-Fa-f]+\(r1\)", body):
        r = int(m.group(1))
        if r >= 14 and r <= 31 and r not in a.callee_saved_gprs:
            a.callee_saved_gprs.append(r)
    for m in re.finditer(r"stfd\s+f(\d+),\s*0x[0-9A-Fa-f]+\(r1\)", body):
        r = int(m.group(1))
        if r >= 14 and r <= 31 and r not in a.callee_saved_fprs:
            a.callee_saved_fprs.append(r)

    # default pragma suggestion: always peephole off + scheduling on
    if "#pragma peephole off" not in "\n".join(a.pragma_suggestions):
        a.pragma_suggestions.insert(0, "#pragma peephole off  // default — defeats branchless ternary lowering")
    if a.has_stmw or a.stack_frame and a.stack_frame >= 0x20:
        a.pragma_suggestions.append("#pragma scheduling on  // larger frames often need load-batch scheduling")

    return a


def emit_skeleton(a: Analysis) -> str:
    fn_name = re.search(r"_fn_([0-9A-Fa-f]+)\.inc$", a.path.name)
    fn = f"fn_{fn_name.group(1)}" if fn_name else "fn_UNKNOWN"

    lines = []
    lines.append(f"# Pattern analysis for {fn} ({a.path.name})")
    lines.append("")
    lines.append(f"- Size: ~0x{a.size_bytes:X} bytes (~{a.size_bytes // 4} instructions)")
    lines.append(f"- Stack frame: 0x{a.stack_frame:X}" if a.stack_frame else "- Stack frame: (leaf?)")
    if a.callee_saved_gprs:
        lines.append(f"- Callee-saved GPRs: {sorted(a.callee_saved_gprs)}")
    if a.callee_saved_fprs:
        lines.append(f"- Callee-saved FPRs: {sorted(a.callee_saved_fprs)}")
    lines.append("")

    if a.skip_reasons:
        lines.append("## ⚠ SKIP reasons (matching likely fails)")
        for r in a.skip_reasons:
            lines.append(f"- {r}")
        lines.append("")

    lines.append("## Suggested pragmas (try in order)")
    for p in a.pragma_suggestions:
        lines.append(f"- {p}")
    lines.append("")

    if a.pattern_hints:
        lines.append("## Detected patterns")
        for h in a.pattern_hints:
            lines.append(f"- {h}")
        lines.append("")

    if a.calls:
        lines.append("## Functions called (check existing externs before adding!)")
        for c in a.calls:
            lines.append(f"- `{c}`")
        lines.append("")

    if a.sda_symbols:
        lines.append("## SDA symbols accessed")
        for s in a.sda_symbols:
            lines.append(f"- `{s}` (sda21 — verify extern type matches usage: f32/u32/u16/u8)")
        lines.append("")

    if a.rodata_symbols:
        lines.append("## Rodata / absolute symbols")
        for s in a.rodata_symbols:
            lines.append(f"- `{s}` (use `extern u8 {s}[]` if indexed)")
        lines.append("")

    if a.jumptable_symbols:
        lines.append("## Jump tables")
        for s in a.jumptable_symbols:
            lines.append(f"- `{s}` — requires `extern void* {s}[];` decl + exact case ordering")
        lines.append("")

    lines.append("## Skeleton (fill in body)")
    lines.append("```c")
    for p in a.pragma_suggestions:
        # extract just the pragma directive
        m = re.match(r"(#pragma [^ ]+( [^ ]+)?)", p)
        if m:
            lines.append(m.group(1))
    # guess signature: void fn(...) — fill in return type after reading asm
    lines.append("s32 " + fn + "(/* TODO args — match existing extern */) {")
    lines.append("    /* TODO body */")
    lines.append("    return 0;")
    lines.append("}")
    lines.append("#pragma pop")
    lines.append("```")
    lines.append("")
    lines.append("## Raw asm (for reference)")
    lines.append("```")
    lines.extend(a.lines)
    lines.append("```")
    return "\n".join(lines)


def main(argv: list[str]) -> int:
    if len(argv) < 2:
        print(__doc__)
        return 2
    target = Path(argv[1])
    if not target.exists():
        print(f"ERROR: {target} not found", file=sys.stderr)
        return 1
    a = analyze(target)
    print(emit_skeleton(a))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))

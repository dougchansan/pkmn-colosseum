#!/usr/bin/env python3
"""mine_xrefs.py - deterministic string<->function xref miner for GC6E01.

Reads dtk's symbol-resolved disassembly (build/GC6E01/asm/auto_*.s) and
produces two artifacts that the downstream symbol-map passes consume:

  strings.json     label -> {addr, section, text, n_pieces}
                   every .obj that contains at least one `.string`/`.asciz`
                   (CodeWarrior __FILE__ literals, GSLog/assert format
                   strings, menu text, SJIS/Japanese text, ...).

  fn_strings.json  fn_name -> {addr, size, refs:[label,...],
                              strings:[{label,text}], file_strings:[...]}
                   every function and the *string* labels it references.
                   References are taken straight from dtk's resolved operands
                   (`lbl_X@ha`, `lbl_X@l`, `name@sda21(r13)`, `bl name`, ...),
                   so small-data-area (r2/r13) refs are handled for free -
                   the exact reason Ghidra needed full auto-analysis.

This is fully deterministic and reproducible: the only input is dtk asm,
regenerable with `python configure.py && ninja` (or `tools/dtk.exe dol split`).
No Ghidra round-trip required.

Usage:
    python tools/symbolmap/mine_xrefs.py \
        --asm-dir <repo>/build/GC6E01/asm \
        --out-dir <repo>/config/GC6E01/symbolmap
"""

import argparse
import json
import re
from pathlib import Path

# A dtk function block:  ".fn NAME, scope" ... ".endfn NAME"
FN_START = re.compile(r"^\.fn\s+(?P<name>\S+?),")
FN_END = re.compile(r"^\.endfn\b")

# A dtk data object:     ".obj LABEL, scope" ... ".endobj LABEL"
OBJ_START = re.compile(r"^\.obj\s+(?P<name>\S+?),")
OBJ_END = re.compile(r"^\.endobj\b")

# The address-bearing comment that precedes every .obj/.fn, e.g.
#   "# .rodata:0xDC38 | 0x80273FD8 | size: 0x2E"
ADDR_COMMENT = re.compile(r"\|\s*0x(?P<addr>[0-9A-Fa-f]{8})\s*\|")

# A string payload line: `.string "..."` or `.asciz "..."`.
STRING_LINE = re.compile(r'^\s*\.(?:string|asciz)\s+"(?P<text>.*)"\s*$')

# Strip dtk's leading `/* ADDR OFF  BYTES */` comment from an instruction line
# so its hex bytes can never be misread as a symbol reference.
LEADING_COMMENT = re.compile(r"^/\*.*?\*/")

# Any symbol token that could be a reference operand. dtk names are
# `lbl_XXXXXXXX`, `fn_XXXXXXXX`, or real identifiers; suffixes @ha/@h/@l/@sda21
# are stripped by the word boundary.
REF_TOKEN = re.compile(r"\b([A-Za-z_][A-Za-z0-9_]*)\b")

# Every dtk label embeds the target's 8-hex address: `lbl_80273FD8`,
# `"@1782_802FBB68"` (auto-split), `_name_804EA658` (auto-named symbol). We key
# references by this address so resolution is independent of label naming and
# works across a proper build (Colosseum) and an auto-split (XD) alike.
REF_ADDR = re.compile(r"8[0-9A-Fa-f]{7}")

# Section kinds that can hold string literals.
DATA_SECTIONS = ("rodata", "data", "sdata", "sdata2")


def parse_strings(asm_dir: Path) -> dict:
    """label -> {addr, section, text, n_pieces} for every string .obj."""
    strings = {}
    for s in sorted(asm_dir.glob("auto_*.s")):
        if not any(k in s.name for k in DATA_SECTIONS):
            continue
        section = next(k for k in DATA_SECTIONS if k in s.name)
        lines = s.read_text(encoding="utf-8", errors="replace").splitlines()
        i = 0
        pending_addr = None
        while i < len(lines):
            line = lines[i]
            m_addr = ADDR_COMMENT.search(line)
            if m_addr:
                pending_addr = m_addr.group("addr")
            m_obj = OBJ_START.match(line)
            if m_obj:
                label = m_obj.group("name")
                addr = pending_addr
                pieces = []
                i += 1
                # Walk the object body collecting any string payloads.
                while i < len(lines) and not OBJ_END.match(lines[i]):
                    m_str = STRING_LINE.match(lines[i])
                    if m_str:
                        pieces.append(m_str.group("text"))
                    i += 1
                if pieces:
                    strings[label] = {
                        "addr": addr,
                        "section": section,
                        "text": "".join(pieces),
                        "n_pieces": len(pieces),
                    }
                pending_addr = None
                continue
            i += 1
    return strings


def parse_functions(asm_dir: Path) -> dict:
    """fn_name -> {addr, n_instr, refs, ref_addrs} from resolved operands.

    `refs`      : identifier tokens (named symbols) used as operands.
    `ref_addrs` : uppercase 8-hex addresses embedded in operand labels, the
                  naming-independent key used to resolve string references.
    """
    funcs = {}
    for s in sorted(asm_dir.glob("auto_*text*.s")):
        lines = s.read_text(encoding="utf-8", errors="replace").splitlines()
        i = 0
        pending_addr = None
        while i < len(lines):
            line = lines[i]
            m_addr = ADDR_COMMENT.search(line)
            if m_addr:
                pending_addr = m_addr.group("addr")
            m_fn = FN_START.match(line)
            if m_fn:
                name = m_fn.group("name")
                addr = pending_addr
                refs = set()
                ref_addrs = set()
                ninstr = 0
                i += 1
                while i < len(lines) and not FN_END.match(lines[i]):
                    body = LEADING_COMMENT.sub("", lines[i]).strip()
                    if body and not body.startswith((".", "#")):
                        ninstr += 1
                        parts = body.split(None, 1)   # operands after mnemonic
                        if len(parts) == 2:
                            ops = parts[1]
                            for tok in REF_TOKEN.findall(ops):
                                refs.add(tok)
                            for a in REF_ADDR.findall(ops):
                                ref_addrs.add(a.upper())
                    i += 1
                funcs[name] = {
                    "addr": addr,
                    "n_instr": ninstr,
                    "refs": sorted(refs),
                    "ref_addrs": sorted(ref_addrs),
                }
                pending_addr = None
                continue
            i += 1
    return funcs


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--asm-dir", required=True, type=Path,
                    help="dtk asm dir, e.g. build/GC6E01/asm")
    ap.add_argument("--out-dir", required=True, type=Path,
                    help="output dir, e.g. config/GC6E01/symbolmap")
    args = ap.parse_args()

    asm_dir = args.asm_dir
    if not asm_dir.is_dir():
        raise SystemExit(f"asm dir not found: {asm_dir} "
                         "(build it with `ninja` / dtk dol split)")
    args.out_dir.mkdir(parents=True, exist_ok=True)

    print(f"[mine] scanning {asm_dir} ...")
    strings = parse_strings(asm_dir)
    print(f"[mine] {len(strings)} string objects")

    # Address -> (label, text) so references resolve regardless of label naming.
    addr2str = {}
    for label, info in strings.items():
        if info["addr"]:
            addr2str[info["addr"].upper()] = (label, info["text"])

    funcs = parse_functions(asm_dir)
    for f in funcs.values():
        hits = [(addr2str[a][0], addr2str[a][1])
                for a in f["ref_addrs"] if a in addr2str]
        f["strings"] = [{"label": l, "text": t} for l, t in hits]
        f["file_strings"] = sorted(
            t for _, t in hits if t.endswith(".c"))
    with_str = sum(1 for f in funcs.values() if f["strings"])
    print(f"[mine] {len(funcs)} functions, {with_str} reference >=1 string")

    (args.out_dir / "strings.json").write_text(
        json.dumps(strings, ensure_ascii=False, indent=1), encoding="utf-8")
    (args.out_dir / "fn_strings.json").write_text(
        json.dumps(funcs, ensure_ascii=False, indent=1), encoding="utf-8")
    print(f"[mine] wrote {args.out_dir/'strings.json'}")
    print(f"[mine] wrote {args.out_dir/'fn_strings.json'}")


if __name__ == "__main__":
    main()

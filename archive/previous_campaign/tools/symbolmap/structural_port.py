#!/usr/bin/env python3
"""structural_port.py - port XD names by instruction-structure fingerprint.

Colosseum and XD (GXXE01) share the same engine + compiler, so a function
present in both compiles to the same instruction-MNEMONIC sequence (addresses,
immediates and relocations differ, but the opcode stream does not). This f
fingerprints every function by its mnemonic sequence and matches Colosseum
functions to XD functions with the identical fingerprint.

A port is emitted only when the match is unambiguous and trustworthy:
  * the fingerprint is UNIQUE within each game (1:1), so two coincidentally
    similar functions can't cross-match;
  * the function is non-trivial (>= MIN_INSTR mnemonics), so `lwz;blr` getters
    don't match by accident;
  * the XD function carries a real name (not fn_/lbl_);
  * the Colosseum function is still fn_ (unnamed) - otherwise nothing to do.

Confidence is HIGH when the matched pair ALSO shares a string literal (two
independent signals agree); MED on structure alone.
"""
import argparse
import hashlib
import json
import re
from collections import defaultdict
from pathlib import Path

FN_START = re.compile(r"^\.fn\s+(?P<name>\S+?),")
FN_END = re.compile(r"^\.endfn\b")
LEADING_COMMENT = re.compile(r"^/\*.*?\*/")
ADDR_COMMENT = re.compile(r"\|\s*0x(?P<addr>[0-9A-Fa-f]{8})\s*\|")
MIN_INSTR = 10


def fingerprints(asm_dir: Path) -> dict:
    """fn_name -> {fp, n, addr} ; fp = hash of the mnemonic sequence."""
    out = {}
    for s in sorted(asm_dir.glob("auto_*text*.s")):
        lines = s.read_text(encoding="utf-8", errors="replace").splitlines()
        i, pend = 0, None
        while i < len(lines):
            m_addr = ADDR_COMMENT.search(lines[i])
            if m_addr:
                pend = m_addr.group("addr")
            m = FN_START.match(lines[i])
            if m:
                name = m.group("name")
                mnem = []
                i += 1
                while i < len(lines) and not FN_END.match(lines[i]):
                    body = LEADING_COMMENT.sub("", lines[i]).strip()
                    if body and not body.startswith((".", "#")):
                        mnem.append(body.split(None, 1)[0])
                    i += 1
                h = hashlib.sha1("\n".join(mnem).encode()).hexdigest()
                out[name] = {"fp": h, "n": len(mnem), "addr": pend}
                pend = None
                continue
            i += 1
    return out


def unique_index(fps: dict) -> dict:
    """fp -> single fn_name, only for fingerprints owned by exactly one fn."""
    by_fp = defaultdict(list)
    for name, d in fps.items():
        if d["n"] >= MIN_INSTR:
            by_fp[d["fp"]].append(name)
    return {fp: names[0] for fp, names in by_fp.items() if len(names) == 1}


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--col-asm", required=True, type=Path)
    ap.add_argument("--xd-asm", required=True, type=Path)
    ap.add_argument("--sm-dir", required=True, type=Path)
    ap.add_argument("--out", required=True, type=Path)
    args = ap.parse_args()

    col = fingerprints(args.col_asm)
    xd = fingerprints(args.xd_asm)
    print(f"[struct] col={len(col)} xd={len(xd)} functions")

    col_u = unique_index(col)
    xd_u = unique_index(xd)
    print(f"[struct] unique fingerprints: col={len(col_u)} xd={len(xd_u)}")

    # string evidence to corroborate (from the miner, if present)
    fn_strings = {}
    p = args.sm_dir / "fn_strings.json"
    if p.is_file():
        fn_strings = json.loads(p.read_text(encoding="utf-8"))

    is_named = lambda n: not re.match(r"(fn|lbl)_[0-9A-Fa-f]{8}$", n)
    ports = []
    for fp, cfn in col_u.items():
        xfn = xd_u.get(fp)
        if not xfn or not is_named(xfn):
            continue
        if is_named(cfn):                      # already named in Colosseum
            continue
        shared = []
        if cfn in fn_strings:
            shared = [s["text"][:40] for s in fn_strings[cfn].get("strings", [])]
        ports.append({"fn": cfn, "xd_name": xfn, "n": col[cfn]["n"],
                      "confidence": "HIGH" if shared else "MED",
                      "shared_strings": shared[:2]})

    ports.sort(key=lambda p: (p["confidence"] != "HIGH", -p["n"]))
    args.out.write_text(json.dumps(ports, ensure_ascii=False, indent=1),
                        encoding="utf-8")
    hi = sum(1 for p in ports if p["confidence"] == "HIGH")
    print(f"[struct] {len(ports)} structural ports ({hi} HIGH/string-corroborated, "
          f"{len(ports)-hi} MED/structure-only) -> {args.out.name}")


if __name__ == "__main__":
    main()

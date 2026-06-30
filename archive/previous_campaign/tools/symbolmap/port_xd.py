#!/usr/bin/env python3
"""port_xd.py - Stage 3: port real names from the XD (GXXE01) decomp.

Pokemon XD: Gale of Darkness runs the same engine as Colosseum and its decomp
maintains ~17k named functions. Two ways to leverage that here:

  (1) VALIDATE  (always runs)
      Cross-check this project's string-derived name proposals (Stage 2)
      against XD's named-function set. A proposal whose name exists verbatim as
      a function in XD is corroborated as a real engine symbol, not a guess.
      Also emits an XD name vocabulary grouped by engine prefix for manual use.

  (2) PORT  (runs when XD asm is available via --xd-asm)
      Mine XD's disassembly into fn -> {string literals} with the same miner,
      then match each Colosseum function to the XD function with the strongest
      DISTINCTIVE shared-string-literal overlap (engine code shares exact
      strings across the two games). When the match is unambiguous and the XD
      function is named, port that real name across.

      XD asm is not committed in the refs tree. To produce it:
        tools/dtk.exe split <xd>/config/GXXE01/config.yml <out>
      (needs XD's orig/GXXE01/sys/main.dol) - then pass <out>/.../asm here.

Outputs (config/GC6E01/symbolmap/):
  xd_port.md, xd_port.json   - validation + (when available) ported names.
"""

import argparse
import json
import re
import sys
from collections import Counter, defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from mine_xrefs import parse_strings, parse_functions  # noqa: E402

PREFIXES = ("GS", "HSD_", "hsd", "waza", "leaffx", "tracefx", "floor", "battle",
            "menu", "snd", "sound", "CARDE", "card", "pkjb", "gba", "poke",
            "OS", "DVD", "GX", "VI", "PAD", "sequence", "eye", "envMap")


def load_named_functions(symbols: Path) -> dict:
    """name -> addr for symbols that are functions and have a real name."""
    out = {}
    fn_re = re.compile(r"^(\S+)\s*=\s*\.\w+:0x([0-9A-Fa-f]+);.*type:function")
    for line in symbols.read_text(encoding="utf-8").splitlines():
        m = fn_re.match(line)
        if m and not re.match(r"(fn|lbl)_[0-9A-Fa-f]{8}$", m.group(1)):
            out[m.group(1)] = m.group(2)
    return out


def mine_game(asm_dir: Path) -> dict:
    """fn_name -> set(string_text) for a game, via the shared miner.

    Resolves references by embedded address so it works for both a proper build
    (Colosseum, `lbl_ADDR` labels) and an auto-split (XD, `"@N_ADDR"` labels).
    """
    strings = parse_strings(asm_dir)
    addr2text = {info["addr"].upper(): info["text"]
                 for info in strings.values() if info["addr"]}
    funcs = parse_functions(asm_dir)
    out = {}
    for name, f in funcs.items():
        texts = {addr2text[a] for a in f["ref_addrs"] if a in addr2text}
        if texts:
            out[name] = texts
    return out


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--sm-dir", required=True, type=Path)
    ap.add_argument("--xd-symbols", required=True, type=Path)
    ap.add_argument("--col-asm", type=Path,
                    help="Colosseum asm dir (for the PORT path)")
    ap.add_argument("--xd-asm", type=Path,
                    help="XD asm dir from `dtk split` (enables the PORT path)")
    args = ap.parse_args()

    xd_named = load_named_functions(args.xd_symbols)
    xd_name_set = set(xd_named)
    proposals = json.loads(
        (args.sm_dir / "name_proposals.json").read_text(encoding="utf-8"))

    # (1) VALIDATE
    for p in proposals:
        p["in_xd"] = p["proposed"] in xd_name_set
    corroborated = [p for p in proposals if p["in_xd"]]

    md = ["# XD (GXXE01) name port & validation", "",
          f"XD named-function vocabulary: {len(xd_named)} symbols.", ""]
    md.append("## Validation of string-derived proposals\n")
    md.append(f"{len(corroborated)} / {len(proposals)} proposed names exist "
              "verbatim as functions in XD (corroborated as real engine "
              "symbols).\n")
    md.append("| fn | proposed | in XD? | confidence |")
    md.append("|---|---|---|---|")
    for p in sorted(proposals, key=lambda x: (not x["in_xd"], x["fn"])):
        md.append(f"| `{p['fn']}` | {p['proposed']} | "
                  f"{'✔' if p['in_xd'] else ''} | {p['confidence']} |")

    ported = []
    # (2) PORT
    if args.xd_asm and args.col_asm and args.xd_asm.is_dir() \
            and args.col_asm.is_dir():
        md.append("\n## Cross-game string-literal port\n")
        col = mine_game(args.col_asm)
        xd = mine_game(args.xd_asm)
        # Index XD functions by each distinctive string they reference. A string
        # referenced by many XD functions is not distinctive; weight by rarity.
        str_xd = defaultdict(list)
        for g, texts in xd.items():
            for t in texts:
                str_xd[t].append(g)
        for f, texts in col.items():
            scores = Counter()
            for t in texts:
                gs = str_xd.get(t, [])
                if 1 <= len(gs) <= 4:            # distinctive shared string
                    for g in gs:
                        scores[g] += 1.0 / len(gs)
            if not scores:
                continue
            g, sc = scores.most_common(1)[0]
            second = scores.most_common(2)[1][1] if len(scores) > 1 else 0
            if sc >= 1.0 and sc > second and g in xd_name_set:
                ported.append({"fn": f, "xd_fn": g, "name": g,
                               "score": round(sc, 2), "shared": sorted(
                                   texts & xd.get(g, set()))[:3]})

        # An XD name claimed by >1 Colosseum function can't disambiguate which
        # variant is which (e.g. flagSet/flagClear/flagGet share one error
        # string). Flag those as ambiguous; keep them out of the apply-list.
        name_fns = defaultdict(list)
        for p in ported:
            name_fns[p["name"]].append(p["fn"])
        for p in ported:
            p["ambiguous"] = len(name_fns[p["name"]]) > 1
            p["col_named"] = not re.match(r"fn_[0-9A-Fa-f]{8}$", p["fn"])
            p["confidence"] = ("HIGH" if p["score"] >= 2 and not p["ambiguous"]
                               else "MED")

        applyable = [p for p in ported if p["confidence"] == "HIGH"
                     and not p["col_named"]]
        md.append(f"{len(ported)} function(s) matched to a named XD function "
                  f"by shared string literals; {len(applyable)} are HIGH-"
                  "confidence, unambiguous and currently unnamed.\n")
        md.append("| col fn | → XD name | score | conf | flags | shared |")
        md.append("|---|---|---|---|---|---|")
        for p in sorted(ported, key=lambda x: (-x["score"], x["fn"])):
            sh = "; ".join(s[:28] for s in p["shared"])
            fl = []
            if p["ambiguous"]:
                fl.append("ambiguous")
            if p["col_named"]:
                fl.append("already-named")
            nm = f"**{p['name']}**" if p["confidence"] == "HIGH" else p["name"]
            md.append(f"| `{p['fn']}` | {nm} | {p['score']} | {p['confidence']} "
                      f"| {','.join(fl)} | `{sh}` |")

        # Apply-able symbol patch: unnamed col fn -> real XD name.
        patch = [f"{p['fn']}  // XD: {p['name']}  (score {p['score']})"
                 for p in sorted(applyable, key=lambda x: x["fn"])]
        (args.sm_dir / "xd_port_apply.txt").write_text(
            "\n".join(patch) + "\n", encoding="utf-8")
    else:
        md.append("\n## Cross-game string-literal port\n")
        md.append("_Skipped: XD asm not supplied. Provide `--xd-asm` "
                  "(from `dtk split` on XD's dol) and `--col-asm` to enable._\n")

    # Vocabulary export grouped by prefix.
    vocab = defaultdict(list)
    for name in xd_named:
        for pre in PREFIXES:
            if name.startswith(pre):
                vocab[pre].append(name)
                break
    (args.sm_dir / "xd_vocabulary.json").write_text(
        json.dumps({k: sorted(v) for k, v in vocab.items()},
                   ensure_ascii=False, indent=1), encoding="utf-8")

    (args.sm_dir / "xd_port.md").write_text("\n".join(md) + "\n",
                                             encoding="utf-8")
    (args.sm_dir / "xd_port.json").write_text(
        json.dumps({"corroborated": corroborated, "ported": ported},
                   ensure_ascii=False, indent=1), encoding="utf-8")
    print(f"[xd] {len(corroborated)}/{len(proposals)} proposals corroborated "
          f"by XD; {len(ported)} ported by string match")
    print(f"[xd] vocabulary: {sum(len(v) for v in vocab.values())} prefixed "
          f"names across {len(vocab)} prefixes")
    print("[xd] wrote xd_port.md / xd_port.json / xd_vocabulary.json")


if __name__ == "__main__":
    main()

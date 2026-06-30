#!/usr/bin/env python3
"""surgical_report_update.py — mark a known set of trust-gated 100% functions
in report.json WITHOUT a full local rebuild.

Why: the local build/measure env is incomplete (effect_visual not in objdiff.json,
HSD W-SDA-WRAPPER TUs don't compile), so a naive gen_decomp_report regen DROPS
~63 functions that the committed report legitimately counts. We instead start
from the committed baseline and flip ONLY the functions we band-verified at 100%
this run, recomputing the affected units + the top-level aggregate.

Usage: python tools/decomp_work/surgical_report_update.py <fns_file> [report.json]
  <fns_file> = newline-separated fn_ names that are now 100% byte-exact.
"""
import json
import subprocess
import sys
from pathlib import Path

MATCH = 99.995  # treat >= as 100% matched
ROOT = Path(__file__).resolve().parents[2]


def ci(v):  # code value -> int (schema stores as string)
    return int(v or 0)


def cf(v):
    try:
        return float(v or 0)
    except (TypeError, ValueError):
        return 0.0


def source_classes():
    sys.path.insert(0, str(ROOT / "tools" / "decomp_work"))
    import progress2  # type: ignore

    _, fn2class = progress2.classify_all()
    return fn2class


def recompute_unit(u, fn2class):
    fns = u.get("functions", [])
    m = u["measures"]
    total_code = ci(m.get("total_code"))
    matched_code = 0
    matched_fns = 0
    fuzzy_weight = 0.0
    for f in fns:
        size = ci(f.get("size"))
        is_real_c = fn2class.get(f.get("name")) == "REAL_C"
        fuzzy = cf(f.get("fuzzy_match_percent")) if is_real_c else 0.0
        if not is_real_c:
            f.pop("fuzzy_match_percent", None)
        fuzzy_weight += size * fuzzy
        if is_real_c and fuzzy >= MATCH:
            matched_code += size
            matched_fns += 1
    total_fns = int(m.get("total_functions", len(fns)) or 0)
    # code-weighted fuzzy
    fuzzy = (fuzzy_weight / total_code) if total_code else 0.0
    m["matched_code"] = str(matched_code)
    m["matched_code_percent"] = (100.0 * matched_code / total_code) if total_code else 0.0
    m["matched_functions"] = matched_fns
    m["matched_functions_percent"] = (100.0 * matched_fns / total_fns) if total_fns else 0.0
    m["fuzzy_match_percent"] = fuzzy
    # a newly byte-exact function is also "complete"
    m["complete_code"] = str(matched_code)
    m["complete_code_percent"] = m["matched_code_percent"]
    m["complete_units"] = 1 if (total_fns and matched_fns == total_fns) else 0
    return u


def main():
    fns_file = sys.argv[1]
    report = sys.argv[2] if len(sys.argv) > 2 else "report.json"
    crk = set(Path(fns_file).read_text().split())
    d = json.loads(Path(report).read_text(encoding="utf-8"))
    fn2class = source_classes()

    touched = set()
    flipped = 0
    for u in d["units"]:
        unit_hit = False
        for f in u.get("functions", []):
            if (
                f["name"] in crk
                and fn2class.get(f["name"]) == "REAL_C"
                and f.get("fuzzy_match_percent", 0) < MATCH
            ):
                f["fuzzy_match_percent"] = 100.0
                flipped += 1
                unit_hit = True
        if unit_hit:
            recompute_unit(u, fn2class)
            touched.add(u["name"])

    # top-level aggregate = sum of units
    units = d["units"]
    T = d["measures"]
    tot_code = sum(ci(u["measures"].get("total_code")) for u in units)
    mat_code = sum(ci(u["measures"].get("matched_code")) for u in units)
    cmp_code = sum(ci(u["measures"].get("complete_code")) for u in units)
    tot_fns = sum(int(u["measures"].get("total_functions", 0) or 0) for u in units)
    mat_fns = sum(int(u["measures"].get("matched_functions", 0) or 0) for u in units)
    cmp_units = sum(int(u["measures"].get("complete_units", 0) or 0) for u in units)
    code_fuzzy_weight = sum(
        ci(u["measures"].get("total_code")) * u["measures"].get("fuzzy_match_percent", 0)
        for u in units
    )
    total_data = ci(T.get("total_data"))
    matched_data = ci(T.get("matched_data"))
    fuzzy = (
        (code_fuzzy_weight + (100.0 * matched_data)) / (tot_code + total_data)
        if (tot_code + total_data)
        else 0.0
    )
    T["total_code"] = str(tot_code)
    T["matched_code"] = str(mat_code)
    T["matched_code_percent"] = (100.0 * mat_code / tot_code) if tot_code else 0.0
    T["complete_code"] = str(cmp_code)
    T["complete_code_percent"] = (100.0 * cmp_code / tot_code) if tot_code else 0.0
    T["total_functions"] = tot_fns
    T["matched_functions"] = mat_fns
    T["matched_functions_percent"] = (100.0 * mat_fns / tot_fns) if tot_fns else 0.0
    T["fuzzy_match_percent"] = fuzzy
    T["complete_units"] = cmp_units

    # match gen_decomp_report.py's minified, no-space encoding so the diff is clean
    Path(report).write_text(json.dumps(d, separators=(",", ":")), encoding="utf-8")
    print(f"flipped {flipped} functions across {len(touched)} units: {sorted(touched)}")
    print(f"TOP: {mat_fns}/{tot_fns} fn ({T['matched_functions_percent']:.2f}%), "
          f"{T['matched_code_percent']:.2f}% code")
    if Path(report).resolve() == (ROOT / "report.json").resolve():
        subprocess.run(
            [sys.executable, "tools/sync_progress_metadata.py", "--sync"],
            cwd=ROOT,
            check=True,
        )


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
"""gen_decomp_report.py - emit a decomp.dev-ingestible objdiff Report JSON.

decomp.dev parses an objdiff "Report" protobuf serialized to JSON. We cannot
produce this in public CI (it needs the copyright ROM-extracted target object),
so we generate it LOCALLY (this machine has the ROM + unified target obj +
per-file base objects), commit the result, and have a workflow upload the
committed file as an artifact named "GC6E01_report" so decomp.dev can ingest it.

We do NOT re-implement diffing and we do NOT use objdiff-cli for the published
honest report: objdiff/decomp.dev do not know which 100% matches are active real
C versus asm wrappers, and the project tracks additional report-only per-file
base objects in config/GC6E01/objdiff_report.json. Instead we REUSE
tools/progress.py's measurement: its measure_cache.diff_funcs() returns the
cached, correct per-function match list for each base .o, and
progress.collect()/summarize() give the aggregate we cross-check against. The
function list is not the full executable section: any target code bytes not
represented by measured functions are emitted as one unmatched
"__unattributed_code" unit so the decomp.dev Code denominator reflects the
actual DOL code size from dtk's build config.

The emitted JSON follows the objdiff Report proto's proto3-JSON encoding as
produced by objdiff-cli (pbjson):
  * field names: snake_case (preserve_proto_field_names)
  * 64-bit ints (uint64 *code/*data, ReportItem.size, addresses): JSON STRINGS
  * uint32 (total_functions, matched_functions, total_units, complete_units,
    version, module_id) and float *_percent: bare JSON numbers
  * default/zero scalars, empty repeated, and None optionals are OMITTED
Top-level keys: measures, units, version, categories.

Usage:
    python tools/gen_decomp_report.py                  # -> build_pc/report.json
    python tools/gen_decomp_report.py -o report.json   # custom output path
    python tools/gen_decomp_report.py --pretty         # indented JSON
    python tools/gen_decomp_report.py --allow-denominator-shrink  # intentional

Schema reference: objdiff-core/protos/report.proto (Report/Measures/ReportUnit/
ReportItem/ReportUnitMetadata/ReportItemMetadata).
"""

import argparse
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
TOOLS = ROOT / "tools"
sys.path.insert(0, str(TOOLS))

import measure_cache  # noqa: E402  (per-function diff, cached)
import progress  # noqa: E402  (collect()/summarize() for cross-validation)

DECOMP_WORK = ROOT / "tools" / "decomp_work"

TARGET_O = ROOT / "build" / "GC6E01" / "obj" / "auto_01_800055E0_text.o"
BASE_DIR = ROOT / "build" / "GC6E01" / "base"
REPORT_OBJDIFF_CFG = ROOT / "config" / "GC6E01" / "objdiff_report.json"
BUILD_CFG = ROOT / "build" / "GC6E01" / "config.json"
DATA_PROGRESS = ROOT / "config" / "GC6E01" / "data_progress.json"
UNATTRIBUTED_CODE_UNIT = "__unattributed_code"

# objdiff report.proto version. Current schema is version 1.
REPORT_VERSION = 1
# A function counts as "complete"/matched at exactly this percent.
COMPLETE_PCT = 100.0
REAL_C_CLASS = "REAL_C"


def _u64(v) -> str:
    """uint64 / ReportItem.size encode as JSON strings (proto3 JSON)."""
    return str(int(v))


def _prune(d: dict) -> dict:
    """Drop None values so optional/default fields are omitted from the JSON,
    matching objdiff-cli's pbjson writer (it skips zero/None/empty fields)."""
    return {k: v for k, v in d.items() if v is not None}


def _measures(total_funcs, matched_funcs, total_code, matched_code, fuzzy,
              total_units, complete_units, total_data=0, matched_data=0,
              complete_data=0, include_zero_data=False) -> dict:
    """Build a Measures dict, omitting zero/default fields (proto3-JSON style).

    matched_code == complete_code here: both are the byte count of functions at
    100%.
    """
    m = {}
    # float *_percent (bare numbers); omit when 0.0
    if fuzzy:
        m["fuzzy_match_percent"] = fuzzy
    # uint64 code (strings); omit when 0
    if total_code:
        m["total_code"] = _u64(total_code)
    if matched_code:
        m["matched_code"] = _u64(matched_code)
    mcp = (100.0 * matched_code / total_code) if total_code else 0.0
    if mcp:
        m["matched_code_percent"] = mcp
    # uint64 data (strings). The top-level report keeps explicit zero matched
    # data fields when total_data is known so decomp.dev renders the Data bar
    # even before any data bytes are verified as matched.
    if total_data:
        m["total_data"] = _u64(total_data)
    if matched_data or (include_zero_data and total_data):
        m["matched_data"] = _u64(matched_data)
    mdp = (100.0 * matched_data / total_data) if total_data else 0.0
    if mdp or (include_zero_data and total_data):
        m["matched_data_percent"] = mdp
    # uint32 function counts (bare numbers); omit when 0
    if total_funcs:
        m["total_functions"] = int(total_funcs)
    if matched_funcs:
        m["matched_functions"] = int(matched_funcs)
    mfp = (100.0 * matched_funcs / total_funcs) if total_funcs else 0.0
    if mfp:
        m["matched_functions_percent"] = mfp
    # complete_code == matched_code (100%-only bytes)
    if matched_code:
        m["complete_code"] = _u64(matched_code)
    if mcp:
        m["complete_code_percent"] = mcp
    cdp = (100.0 * complete_data / total_data) if total_data else 0.0
    if complete_data or (include_zero_data and total_data):
        m["complete_data"] = _u64(complete_data)
    if cdp or (include_zero_data and total_data):
        m["complete_data_percent"] = cdp
    if total_units:
        m["total_units"] = int(total_units)
    if complete_units:
        m["complete_units"] = int(complete_units)
    return m


def _load_cfg_map():
    """rel-base-.o-path -> {name, source_path} from the curated report map.

    dtk-template owns the top-level objdiff.json and regenerates it from the
    active link graph. This report map is separate because the public decomp.dev
    snapshot tracks the per-file base objects used for decomp work.
    """
    mapping = {}
    try:
        cfg = json.loads(REPORT_OBJDIFF_CFG.read_text(encoding="utf-8"))
    except (OSError, ValueError):
        return mapping
    for u in cfg.get("units", []):
        bp = u.get("base_path", "") or ""
        marker = "build/GC6E01/base/"
        rel = bp.split(marker, 1)[1] if marker in bp else bp
        if not rel:
            continue
        meta = u.get("metadata") or {}
        mapping[rel] = {
            "name": u.get("name"),
            "source_path": meta.get("source_path"),
        }
    return mapping


def _loadable_data_size() -> int:
    """Return target loadable non-code data bytes from dtk's build config.

    build/GC6E01/config.json is generated by dtk from the ROM split and records
    the section object sizes. decomp.dev's Data bar tracks loadable data/rodata
    sections, not BSS/SBSS, so exclude generated bss/sbss section objects.
    """
    try:
        cfg = json.loads(BUILD_CFG.read_text(encoding="utf-8"))
    except (OSError, ValueError):
        return 0

    total = 0
    for unit in cfg.get("units", []):
        try:
            size = int(unit.get("data_size", 0) or 0)
        except (TypeError, ValueError):
            size = 0
        if not size:
            continue
        name = str(unit.get("name", "")).lower()
        obj = str(unit.get("object", "")).lower()
        if "bss" in name or "bss" in obj:
            continue
        total += size
    return total


def _target_code_size() -> int:
    """Return total executable target bytes from dtk's build config."""
    try:
        cfg = json.loads(BUILD_CFG.read_text(encoding="utf-8"))
    except (OSError, ValueError):
        return 0

    total = 0
    for unit in cfg.get("units", []):
        try:
            total += int(unit.get("code_size", 0) or 0)
        except (TypeError, ValueError):
            continue
    return total


def _data_progress() -> tuple[int, int]:
    """Return (matched_data, complete_data) from the data progress manifest."""
    try:
        progress_data = json.loads(DATA_PROGRESS.read_text(encoding="utf-8"))
    except (OSError, ValueError):
        return 0, 0

    matched = 0
    complete = 0
    seen = set()
    for item in progress_data.get("matched", []):
        key = (
            item.get("section"),
            item.get("start"),
            item.get("object"),
            item.get("source_path"),
        )
        if key in seen:
            continue
        seen.add(key)
        try:
            size = int(item.get("size", 0) or 0)
        except (TypeError, ValueError):
            size = 0
        if size <= 0:
            continue
        matched += size
        complete += size
    return matched, complete


def _source_classes() -> dict[str, str]:
    """Return active-source classification by function name.

    Only active REAL_C functions are honest decompilation matches. Active asm
    wrappers and no-source split asm can reproduce ROM bytes, but they are not C
    decompilation and must not contribute matched_functions/matched_code.
    """
    sys.path.insert(0, str(DECOMP_WORK))
    import progress2  # noqa: WPS433  (lazy import avoids a hard tool startup cost)

    _, fn2class = progress2.classify_all()
    return fn2class


def _unit_identity(rel: str, cfg_map: dict):
    """Return (unit_name, source_path) for a base .o rel path.

    Prefer the curated report-map entry; otherwise derive a stable name from the
    rel path (drop the .o) and a best-guess source path src/<rel>.c.
    """
    info = cfg_map.get(rel)
    stem = rel[:-2] if rel.endswith(".o") else rel  # strip ".o"
    if info and info.get("name"):
        name = info["name"]
    else:
        name = stem
    src = info.get("source_path") if info else None
    if not src:
        src = "src/" + stem + ".c"
    return name, src


def build_report():
    cfg_map = _load_cfg_map()
    fn2class = _source_classes()
    g_target_code = _target_code_size()
    g_total_data = _loadable_data_size()
    g_matched_data, g_complete_data = _data_progress()

    units = []
    # Running aggregate. Matched/fuzzy code comes from measured functions; total
    # code is corrected below to the full executable section size.
    g_total_funcs = g_matched_funcs = 0
    g_total_code = g_matched_code = 0
    g_fuzzy_weighted = 0.0  # sum(match% * size)
    complete_units = 0

    base_objects = list(progress.iter_base_objects())
    seen_rel = set()
    for idx, (rel, o) in enumerate(base_objects, 1):
        # Skip phantom doubled-path objects: legit base objects live at
        # base/<game|dolphin|hsd|...>/..., never base/src/... A stale build glitch
        # produced base/src/<path>.o duplicates that double-count units (and show
        # up as duplicate treemap tiles). Guard here so report stays correct even
        # if the artifacts reappear.
        rel_norm = rel.replace("\\", "/")
        if rel_norm.startswith("src/"):
            continue
        if rel_norm in seen_rel:
            continue
        seen_rel.add(rel_norm)
        funcs = measure_cache.diff_funcs(TARGET_O, o)
        if not funcs:  # no fn_ symbols measurable here -> not a report unit
            continue

        name, source_path = _unit_identity(rel, cfg_map)

        items = []
        u_total = len(funcs)
        u_matched = 0
        u_total_code = 0
        u_matched_code = 0
        u_fuzzy_weighted = 0.0
        for f in funcs:
            pct = float(f["match"])
            sz = int(f["size"])
            cls = fn2class.get(f["name"], "NO_SOURCE")
            is_real_c = cls == REAL_C_CLASS
            honest_pct = pct if is_real_c else 0.0
            u_total_code += sz
            u_fuzzy_weighted += honest_pct * sz
            is_complete = is_real_c and pct >= COMPLETE_PCT
            if is_complete:
                u_matched += 1
                u_matched_code += sz
            item = {
                "name": f["name"],
                # ReportItem.size is uint64 -> string; omit if 0
                "size": _u64(sz) if sz else None,
                # fuzzy_match_percent is float -> bare; omit if 0.0
                "fuzzy_match_percent": honest_pct if honest_pct else None,
            }
            items.append(_prune(item))

        u_fuzzy = (u_fuzzy_weighted / u_total_code) if u_total_code else 0.0
        unit_complete = (u_total > 0 and u_matched == u_total)
        if unit_complete:
            complete_units += 1

        unit_measures = _measures(
            total_funcs=u_total, matched_funcs=u_matched,
            total_code=u_total_code, matched_code=u_matched_code,
            fuzzy=u_fuzzy, total_units=1,
            complete_units=1 if unit_complete else 0,
        )

        metadata = _prune({
            "complete": True if unit_complete else None,  # omit when false
            "source_path": source_path,
        })

        unit = _prune({
            "name": name,
            "measures": unit_measures,
            "functions": items,
            "metadata": metadata if metadata else None,
        })
        units.append(unit)

        g_total_funcs += u_total
        g_matched_funcs += u_matched
        g_total_code += u_total_code
        g_matched_code += u_matched_code
        g_fuzzy_weighted += u_fuzzy_weighted

        if idx % 10 == 0:
            try:
                measure_cache.flush()
            except Exception:  # noqa: BLE001
                pass
        if idx % 25 == 0:
            print("measured %d/%d base objects (%d report units)" %
                  (idx, len(base_objects), len(units)), flush=True)

    measured_code = g_total_code
    unattributed_code = max(0, g_target_code - measured_code)
    if unattributed_code:
        units.append({
            "name": UNATTRIBUTED_CODE_UNIT,
            "measures": _measures(
                total_funcs=0, matched_funcs=0,
                total_code=unattributed_code, matched_code=0,
                fuzzy=0.0, total_units=1, complete_units=0,
            ),
            "functions": [],
            "metadata": {
                "source_path": "unattributed/unsplit executable code",
            },
        })
        g_total_code += unattributed_code

    total_units = len(units)
    g_code_fuzzy = (g_fuzzy_weighted / g_total_code) if g_total_code else 0.0
    g_fuzzy = (
        (g_fuzzy_weighted + (100.0 * g_matched_data))
        / (g_total_code + g_total_data)
        if (g_total_code + g_total_data) else 0.0
    )

    measures = _measures(
        total_funcs=g_total_funcs, matched_funcs=g_matched_funcs,
        total_code=g_total_code, matched_code=g_matched_code,
        fuzzy=g_fuzzy, total_units=total_units, complete_units=complete_units,
        total_data=g_total_data, matched_data=g_matched_data,
        complete_data=g_complete_data, include_zero_data=True,
    )

    report = {
        "measures": measures,
        "units": units,
        "version": REPORT_VERSION,
    }
    # categories omitted (empty repeated) per proto3-JSON omit-default rule.

    agg = {
        "total_functions": g_total_funcs,
        "matched_functions": g_matched_funcs,
        "total_code": g_total_code,
        "matched_code": g_matched_code,
        "measured_code": measured_code,
        "unattributed_code": unattributed_code,
        "fuzzy_match_percent": g_fuzzy,
        "code_fuzzy_match_percent": g_code_fuzzy,
        "total_data": g_total_data,
        "matched_data": g_matched_data,
        "complete_data": g_complete_data,
        "total_units": total_units,
        "complete_units": complete_units,
    }
    return report, agg


def validate(report, agg):
    """Cross-check the report against progress.py's own collect()/summarize().
    Returns (ok, lines)."""
    lines = []
    ok = True

    # 1) JSON round-trips
    try:
        s = json.dumps(report)
        json.loads(s)
        lines.append("json: parses OK (%d bytes)" % len(s))
    except Exception as e:  # noqa: BLE001
        ok = False
        lines.append("json: FAILED to round-trip: %r" % e)

    # 2) top-level + measures keys match researched format
    top = set(report.keys())
    expect_top = {"measures", "units", "version"}
    if expect_top.issubset(top):
        lines.append("top-level keys: %s (ok)" % sorted(top))
    else:
        ok = False
        lines.append("top-level keys MISSING: %s" % (expect_top - top))
    mk = set(report["measures"].keys())
    needed = {"total_functions", "matched_functions", "total_code",
              "matched_code", "total_data", "fuzzy_match_percent",
              "total_units"}
    if needed.issubset(mk):
        lines.append("measures keys: present (%d keys)" % len(mk))
    else:
        ok = False
        lines.append("measures keys MISSING: %s" % (needed - mk))

    # 3) uint64 encoded as strings, uint32/float as numbers
    m = report["measures"]
    enc_ok = (isinstance(m.get("total_code"), str)
              and isinstance(m.get("matched_code"), str)
              and isinstance(m.get("total_data"), str)
              and isinstance(m.get("total_functions"), int)
              and isinstance(m.get("fuzzy_match_percent"), float))
    lines.append("encoding: total_code/total_data are str, "
                 "total_functions is int, fuzzy is float -> %s"
                 % ("ok" if enc_ok else "WRONG"))
    ok = ok and enc_ok

    # 4) units_count ~= base .o with functions, plus optional unmatched code bucket.
    units_count = len(report["units"])
    unattributed_units = sum(
        1 for u in report["units"] if u.get("name") == UNATTRIBUTED_CODE_UNIT
    )
    measured_units_count = units_count - unattributed_units
    lines.append(
        "units: %d (%d measured + %d unattributed)"
        % (units_count, measured_units_count, unattributed_units)
    )

    # 5) overall totals AGREE with progress.py collect()/summarize()
    files = progress.collect()
    psum = progress.summarize(files)
    pf_total = psum["total_functions"]
    pf_matched = psum["matched_functions"]
    pf_units = sum(1 for f in files.values() if f["functions"] > 0)
    same_fns = agg["total_functions"] == pf_total
    same_units = (measured_units_count == pf_units)
    lines.append("progress.py: %d/%d functions, %d units"
                 % (pf_matched, pf_total, pf_units))
    lines.append("report:      %d/%d functions, %d units"
                 % (agg["matched_functions"], agg["total_functions"],
                    units_count))
    lines.append(
        "function denominator agrees: %s ; units agree: %s ; "
        "strict real-C matches <= objdiff matches: %s"
        % (same_fns, same_units, agg["matched_functions"] <= pf_matched)
    )

    measured_code = psum["total_bytes"]
    target_code = _target_code_size()
    expected_code = max(target_code, measured_code)
    same_code = (
        agg["measured_code"] == measured_code
        and agg["matched_code"] <= psum["matched_bytes"]
        and agg["total_code"] == expected_code
    )
    lines.append(
        "code denominator: measured=%d, target=%d, report=%d; "
        "strict matched=%d <= objdiff matched=%d -> %s"
        % (
            measured_code,
            target_code,
            agg["total_code"],
            agg["matched_code"],
            psum["matched_bytes"],
            "ok" if same_code else "WRONG",
        )
    )
    ok = ok and same_fns and same_units and same_code

    # 6) sanity: matched <= total, percent in [0,100]
    sane = (agg["matched_functions"] <= agg["total_functions"]
            and agg["matched_code"] <= agg["total_code"]
            and agg["matched_data"] <= agg["total_data"]
            and agg["complete_data"] <= agg["total_data"]
            and 0.0 <= agg["fuzzy_match_percent"] <= 100.0)
    lines.append("sanity (matched<=total, data<=total, 0<=fuzzy<=100): %s"
                 % ("ok" if sane else "WRONG"))
    ok = ok and sane

    return ok, lines


def _measure_int(measures: dict, key: str) -> int:
    try:
        return int(float(measures.get(key, 0) or 0))
    except (TypeError, ValueError):
        return 0


def _check_denominator_shrink(out: Path, agg: dict, allow: bool) -> None:
    """Refuse to replace a fuller report with a partial local build by default."""
    if allow or not out.exists():
        return
    try:
        existing = json.loads(out.read_text(encoding="utf-8"))
    except (OSError, ValueError):
        return
    measures = existing.get("measures", {}) or {}
    old_functions = _measure_int(measures, "total_functions")
    old_units = _measure_int(measures, "total_units")
    old_code = _measure_int(measures, "total_code")
    new_functions = int(agg.get("total_functions", 0) or 0)
    new_units = int(agg.get("total_units", 0) or 0)
    new_code = int(agg.get("total_code", 0) or 0)
    if old_functions > new_functions or old_units > new_units or old_code > new_code:
        sys.exit(
            "refusing to overwrite %s: generated report has a smaller "
            "denominator (%d functions/%d units/%d code bytes) than the "
            "existing report (%d functions/%d units/%d code bytes). Your local "
            "build/GC6E01/base may be partial; rerun the full local setup or pass "
            "--allow-denominator-shrink for an intentional denominator cleanup."
            % (
                out,
                new_functions,
                new_units,
                new_code,
                old_functions,
                old_units,
                old_code,
            )
        )


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("-o", "--out", default=str(ROOT / "build_pc" / "report.json"),
                    help="output path (default build_pc/report.json)")
    ap.add_argument("--pretty", action="store_true",
                    help="indent the JSON (still ingestible)")
    ap.add_argument("--allow-denominator-shrink", action="store_true",
                    help="allow replacing an existing report with fewer units/functions")
    args = ap.parse_args()

    if not TARGET_O.exists():
        sys.exit("target .o missing: %s" % TARGET_O)
    if not BASE_DIR.exists():
        sys.exit("base dir missing: %s" % BASE_DIR)

    report, agg = build_report()

    out = Path(args.out)
    _check_denominator_shrink(out, agg, args.allow_denominator_shrink)
    out.parent.mkdir(parents=True, exist_ok=True)
    if args.pretty:
        out.write_text(json.dumps(report, indent=2), encoding="utf-8")
    else:
        out.write_text(json.dumps(report, separators=(",", ":")),
                       encoding="utf-8")

    # persist any new cache entries warmed during this run
    try:
        measure_cache.flush()
    except Exception:  # noqa: BLE001
        pass

    print("wrote %s (%d units)" % (out, len(report["units"])))
    m = report["measures"]
    print("overall: %s/%s functions (%.2f%%), %s/%s code bytes, "
          "%s/%s data bytes, fuzzy %.4f%% (code-only %.4f%%)"
          % (m.get("matched_functions", 0), m.get("total_functions", 0),
             100.0 * agg["matched_functions"] / agg["total_functions"]
             if agg["total_functions"] else 0.0,
             m.get("matched_code", "0"), m.get("total_code", "0"),
             m.get("matched_data", "0"), m.get("total_data", "0"),
             agg["fuzzy_match_percent"], agg["code_fuzzy_match_percent"]))

    print("\n--- validation ---")
    ok, lines = validate(report, agg)
    for ln in lines:
        print("  " + ln)
    print("RESULT: %s" % ("PASS" if ok else "FAIL"))
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())

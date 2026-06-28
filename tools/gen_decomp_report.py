#!/usr/bin/env python3
"""gen_decomp_report.py - emit a decomp.dev-ingestible objdiff Report JSON.

decomp.dev parses an objdiff "Report" protobuf serialized to JSON. We cannot
produce this in public CI (it needs the copyright ROM-extracted target object),
so we generate it LOCALLY (this machine has the ROM + unified target obj +
per-file base objects), commit the result, and have a workflow upload the
committed file as an artifact named "GC6E01_report" so decomp.dev can ingest it.

We do NOT re-implement diffing and we do NOT use objdiff-cli for the report
(our objdiff.json mixes per-file target paths with a single UNIFIED target, so
objdiff-cli would mis-count). Instead we REUSE tools/progress.py's measurement:
its measure_cache.diff_funcs() returns the cached, correct per-function match
list for each base .o, and progress.collect()/summarize() give the aggregate we
cross-check against.

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

TARGET_O = ROOT / "build" / "GC6E01" / "obj" / "auto_01_800055E0_text.o"
BASE_DIR = ROOT / "build" / "GC6E01" / "base"
OBJDIFF_CFG = ROOT / "objdiff.json"
BUILD_CFG = ROOT / "build" / "GC6E01" / "config.json"

# objdiff report.proto version. Version 2 has first-class unit counts and
# complete/linked code/data fields used by decomp.dev's two-bar UI.
REPORT_VERSION = 2
# A function counts as "complete"/matched at exactly this percent.
COMPLETE_PCT = 100.0


def _u64(v) -> str:
    """uint64 / ReportItem.size encode as JSON strings (proto3 JSON)."""
    return str(int(v))


def _prune(d: dict) -> dict:
    """Drop None values so optional/default fields are omitted from the JSON,
    matching objdiff-cli's pbjson writer (it skips zero/None/empty fields)."""
    return {k: v for k, v in d.items() if v is not None}


def _measures(total_funcs, matched_funcs, total_code, matched_code, fuzzy,
              total_units, complete_units, complete_code=0, total_data=0,
              matched_data=0, complete_data=0) -> dict:
    """Build a Measures dict, omitting zero/default fields (proto3-JSON style).

    matched_code is byte-exact code. complete_code/data are completed or linked
    units, matching objdiff report v2 / decomp.dev's two-bar semantics.
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
    if total_data:
        m["total_data"] = _u64(total_data)
    if matched_data:
        m["matched_data"] = _u64(matched_data)
    mdp = (100.0 * matched_data / total_data) if total_data else 0.0
    if mdp:
        m["matched_data_percent"] = mdp
    # uint32 function counts (bare numbers); omit when 0
    if total_funcs:
        m["total_functions"] = int(total_funcs)
    if matched_funcs:
        m["matched_functions"] = int(matched_funcs)
    mfp = (100.0 * matched_funcs / total_funcs) if total_funcs else 0.0
    if mfp:
        m["matched_functions_percent"] = mfp
    if complete_code:
        m["complete_code"] = _u64(complete_code)
    ccp = (100.0 * complete_code / total_code) if total_code else 0.0
    if ccp:
        m["complete_code_percent"] = ccp
    if complete_data:
        m["complete_data"] = _u64(complete_data)
    cdp = (100.0 * complete_data / total_data) if total_data else 0.0
    if cdp:
        m["complete_data_percent"] = cdp
    if total_units:
        m["total_units"] = int(total_units)
    if complete_units:
        m["complete_units"] = int(complete_units)
    return m


def _load_data_units():
    """Return report units for non-code data sections from the local build config.

    The report stores only section sizes and completion metadata, never extracted
    ROM bytes. Autogenerated data units are locally generated from the user's
    extracted data, so they are represented as matched report metadata. They are
    not marked complete unless the build config explicitly says so.
    """
    try:
        cfg = json.loads(BUILD_CFG.read_text(encoding="utf-8"))
    except (OSError, ValueError):
        return [], 0, 0, 0, 0

    raw_units = list(cfg.get("units", []) or [])
    for module in cfg.get("modules", []) or []:
        raw_units.extend(module.get("units", []) or [])

    units = []
    total_data = matched_data = complete_data = 0
    complete_units = 0
    for u in raw_units:
        size = int(u.get("data_size") or 0)
        if size <= 0:
            continue
        auto_generated = bool(u.get("autogenerated"))
        complete = bool(u.get("complete"))
        matched = size if (auto_generated or complete) else 0
        completed = size if complete else 0
        total_data += size
        matched_data += matched
        complete_data += completed
        if complete:
            complete_units += 1
        name = u.get("name") or Path(u.get("object", "data")).stem
        metadata = _prune({
            "complete": True if complete else None,
            "auto_generated": True if auto_generated else None,
            "source_path": u.get("object"),
        })
        units.append(_prune({
            "name": name,
            "measures": _measures(
                total_funcs=0, matched_funcs=0,
                total_code=0, matched_code=0, fuzzy=100.0,
                total_units=1, complete_units=1 if complete else 0,
                total_data=size, matched_data=matched,
                complete_data=completed,
            ),
            "sections": [{
                "name": name,
                "size": _u64(size),
                "fuzzy_match_percent": 100.0 if matched == size else 0.0,
            }],
            "functions": [],
            "metadata": metadata if metadata else None,
        }))
    return units, total_data, matched_data, complete_data, complete_units


def _load_cfg_map():
    """rel-base-.o-path -> {name, source_path} from objdiff.json (curated subset)."""
    mapping = {}
    try:
        cfg = json.loads(OBJDIFF_CFG.read_text(encoding="utf-8"))
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


def _unit_identity(rel: str, cfg_map: dict):
    """Return (unit_name, source_path) for a base .o rel path.

    Prefer the curated objdiff.json entry; otherwise derive a stable name from
    the rel path (drop the .o) and a best-guess source path src/<rel>.c.
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
    data_units, g_total_data, g_matched_data, g_complete_data, data_complete_units = _load_data_units()

    units = list(data_units)
    # running aggregate, computed from the SAME per-function data as the units
    g_total_funcs = g_matched_funcs = 0
    g_total_code = g_matched_code = 0
    g_complete_code = 0
    g_fuzzy_weighted = 0.0  # sum(match% * size)
    complete_units = data_complete_units

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
            u_total_code += sz
            u_fuzzy_weighted += pct * sz
            is_complete = pct >= COMPLETE_PCT
            if is_complete:
                u_matched += 1
                u_matched_code += sz
            item = {
                "name": f["name"],
                # ReportItem.size is uint64 -> string; omit if 0
                "size": _u64(sz) if sz else None,
                # fuzzy_match_percent is float -> bare; omit if 0.0
                "fuzzy_match_percent": pct if pct else None,
            }
            items.append(_prune(item))

        u_fuzzy = (u_fuzzy_weighted / u_total_code) if u_total_code else 0.0
        unit_complete = (u_total > 0 and u_matched == u_total)
        if unit_complete:
            complete_units += 1
            g_complete_code += u_total_code

        unit_measures = _measures(
            total_funcs=u_total, matched_funcs=u_matched,
            total_code=u_total_code, matched_code=u_matched_code,
            fuzzy=u_fuzzy, total_units=1,
            complete_units=1 if unit_complete else 0,
            complete_code=u_total_code if unit_complete else 0,
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

    total_units = len(units)
    g_fuzzy = (g_fuzzy_weighted / g_total_code) if g_total_code else 0.0

    measures = _measures(
        total_funcs=g_total_funcs, matched_funcs=g_matched_funcs,
        total_code=g_total_code, matched_code=g_matched_code,
        fuzzy=g_fuzzy, total_units=total_units, complete_units=complete_units,
        complete_code=g_complete_code,
        total_data=g_total_data, matched_data=g_matched_data,
        complete_data=g_complete_data,
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
        "complete_code": g_complete_code,
        "total_data": g_total_data,
        "matched_data": g_matched_data,
        "complete_data": g_complete_data,
        "fuzzy_match_percent": g_fuzzy,
        "total_units": total_units,
        "complete_units": complete_units,
        "code_units": total_units - len(data_units),
        "data_units": len(data_units),
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
    if report.get("version") != REPORT_VERSION:
        ok = False
        lines.append("version: %s (expected %s)" %
                     (report.get("version"), REPORT_VERSION))
    else:
        lines.append("version: %s (ok)" % report.get("version"))
    mk = set(report["measures"].keys())
    needed = {"total_functions", "matched_functions", "total_code",
              "matched_code", "complete_code", "fuzzy_match_percent",
              "total_units", "total_data", "matched_data"}
    if needed.issubset(mk):
        lines.append("measures keys: present (%d keys)" % len(mk))
    else:
        ok = False
        lines.append("measures keys MISSING: %s" % (needed - mk))

    # 3) uint64 encoded as strings, uint32/float as numbers
    m = report["measures"]
    enc_ok = (isinstance(m.get("total_code"), str)
              and isinstance(m.get("matched_code"), str)
              and (m.get("total_data") is None or isinstance(m.get("total_data"), str))
              and isinstance(m.get("total_functions"), int)
              and isinstance(m.get("fuzzy_match_percent"), float))
    lines.append("encoding: total_code/data are str, total_functions is int, "
                 "fuzzy is float -> %s" % ("ok" if enc_ok else "WRONG"))
    ok = ok and enc_ok

    # 4) units_count ~= base .o with functions
    units_count = len(report["units"])
    lines.append("units: %d" % units_count)

    # 5) overall totals AGREE with progress.py collect()/summarize()
    files = progress.collect()
    psum = progress.summarize(files)
    pf_total = psum["total_functions"]
    pf_matched = psum["matched_functions"]
    pf_units = sum(1 for f in files.values() if f["functions"] > 0)
    same_fns = (agg["total_functions"] == pf_total
                and agg["matched_functions"] == pf_matched)
    same_units = (agg["code_units"] == pf_units)
    lines.append("progress.py: %d/%d functions, %d units"
                 % (pf_matched, pf_total, pf_units))
    lines.append("report:      %d/%d functions, %d code units + %d data units = %d units"
                 % (agg["matched_functions"], agg["total_functions"],
                    agg["code_units"], agg["data_units"], units_count))
    lines.append("functions agree: %s ; code units agree: %s"
                 % (same_fns, same_units))
    ok = ok and same_fns and same_units

    # 6) sanity: matched <= total, percent in [0,100]
    sane = (agg["matched_functions"] <= agg["total_functions"]
            and agg["matched_code"] <= agg["total_code"]
            and agg["complete_code"] <= agg["total_code"]
            and agg["matched_data"] <= agg["total_data"]
            and agg["complete_data"] <= agg["total_data"]
            and 0.0 <= agg["fuzzy_match_percent"] <= 100.0)
    lines.append("sanity (matched<=total, 0<=fuzzy<=100): %s"
                 % ("ok" if sane else "WRONG"))
    ok = ok and sane

    return ok, lines


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("-o", "--out", default=str(ROOT / "build_pc" / "report.json"),
                    help="output path (default build_pc/report.json)")
    ap.add_argument("--pretty", action="store_true",
                    help="indent the JSON (still ingestible)")
    args = ap.parse_args()

    if not TARGET_O.exists():
        sys.exit("target .o missing: %s" % TARGET_O)
    if not BASE_DIR.exists():
        sys.exit("base dir missing: %s" % BASE_DIR)

    report, agg = build_report()

    out = Path(args.out)
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
    print("overall: %s/%s functions (%.2f%%), matched %s/%s code bytes, "
          "linked %s/%s code bytes, matched %s/%s data bytes, fuzzy %.4f%%"
          % (m.get("matched_functions", 0), m.get("total_functions", 0),
             100.0 * agg["matched_functions"] / agg["total_functions"]
             if agg["total_functions"] else 0.0,
             m.get("matched_code", "0"), m.get("total_code", "0"),
             m.get("complete_code", "0"), m.get("total_code", "0"),
             m.get("matched_data", "0"), m.get("total_data", "0"),
             agg["fuzzy_match_percent"]))

    print("\n--- validation ---")
    ok, lines = validate(report, agg)
    for ln in lines:
        print("  " + ln)
    print("RESULT: %s" % ("PASS" if ok else "FAIL"))
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())

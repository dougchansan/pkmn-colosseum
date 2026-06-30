#!/usr/bin/env python3
"""Parity compare: pair a Dolphin (reference) capture with a pcport capture of
the same scene and emit a side-by-side HTML gallery + similarity metrics.

Exact pixel match is NOT expected across two different renderers (Dolphin's
hardware GX vs the PC port's host GL), so this reports structural similarity:
  - dHash distance (perceptual; small = same composition)
  - per-channel mean absolute difference (brightness/color drift)
and leaves the visual verdict to a human via the gallery.

Usage:
  python compare.py --ref dolphin.png --port pcport.png --label outskirt_stand
  python compare.py --manifest pairs.json        # batch (list of {label,ref,port})

Outputs into out/ (default tools/dolphin_harness/scenarios/out/):
  <label>_ref.png, <label>_port.png (normalized to same size), report.html, report.json
"""
import argparse
import json
import os

from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
DEFAULT_OUT = os.path.join(HERE, "out")


def dhash(img, size=8):
    """64-bit difference hash (rows of adjacent-pixel comparisons)."""
    g = img.convert("L").resize((size + 1, size), Image.LANCZOS)
    px = g.load()
    bits = 0
    i = 0
    for y in range(size):
        for x in range(size):
            bits |= (1 if px[x, y] < px[x + 1, y] else 0) << i
            i += 1
    return bits


def hamming(a, b):
    return bin(a ^ b).count("1")


def channel_mad(a, b):
    """Mean absolute difference per RGB channel after resizing b to a's size."""
    if b.size != a.size:
        b = b.resize(a.size, Image.LANCZOS)
    a = a.convert("RGB")
    b = b.convert("RGB")
    ap, bp = a.load(), b.load()
    w, h = a.size
    step = max(1, (w * h) // 20000)  # sample for speed
    sums = [0, 0, 0]
    n = 0
    idx = 0
    for y in range(h):
        for x in range(w):
            idx += 1
            if idx % step:
                continue
            pa, pb = ap[x, y], bp[x, y]
            for c in range(3):
                sums[c] += abs(pa[c] - pb[c])
            n += 1
    return [round(s / n, 2) for s in sums] if n else [0, 0, 0]


def compare_one(label, ref_path, port_path, out_dir):
    ref = Image.open(ref_path)
    port = Image.open(port_path)
    # Normalize the port capture to the reference resolution for display + metrics.
    port_n = port.resize(ref.size, Image.LANCZOS) if port.size != ref.size else port
    ref_out = os.path.join(out_dir, f"{label}_ref.png")
    port_out = os.path.join(out_dir, f"{label}_port.png")
    ref.save(ref_out)
    port_n.save(port_out)
    d = hamming(dhash(ref), dhash(port))
    mad = channel_mad(ref, port)
    return {
        "label": label,
        "ref": os.path.basename(ref_out),
        "port": os.path.basename(port_out),
        "ref_size": list(ref.size),
        "port_size_raw": list(port.size),
        "dhash_distance": d,          # 0 = identical composition, 64 = max
        "channel_mad": mad,           # 0-255 per R,G,B
    }


HTML_HEAD = """<!doctype html><meta charset=utf-8>
<title>pcport parity gallery</title>
<style>
 body{font-family:system-ui,sans-serif;background:#111;color:#ddd;margin:24px}
 h1{font-weight:600} .pair{margin:32px 0;border-top:1px solid #333;padding-top:16px}
 .imgs{display:flex;gap:16px;flex-wrap:wrap} figure{margin:0}
 img{max-width:480px;border:1px solid #444;background:#000} figcaption{font-size:13px;color:#9ad}
 .m{font-size:13px;color:#bbb;margin-top:8px} .m b{color:#fff}
 .warn{color:#f88}
</style>
<h1>PC-port vs Dolphin parity</h1>
<p>Left = Dolphin (reference). Right = PC port. Cross-renderer, so judge composition
and color drift, not exact pixels. dHash distance: 0 = same composition.</p>
"""


def write_html(results, out_dir):
    parts = [HTML_HEAD]
    for r in results:
        warn = " class=warn" if r["dhash_distance"] > 20 else ""
        parts.append(f"""
<div class=pair>
 <h2{warn}>{r['label']} &mdash; dHash {r['dhash_distance']}/64</h2>
 <div class=imgs>
  <figure><img src="{r['ref']}"><figcaption>Dolphin (reference)</figcaption></figure>
  <figure><img src="{r['port']}"><figcaption>PC port</figcaption></figure>
 </div>
 <div class=m>channel MAD (R,G,B): <b>{r['channel_mad']}</b> &nbsp;|&nbsp;
   ref {r['ref_size']} &nbsp; port raw {r['port_size_raw']}</div>
</div>""")
    path = os.path.join(out_dir, "report.html")
    with open(path, "w", encoding="utf-8") as f:
        f.write("".join(parts))
    return path


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--ref")
    ap.add_argument("--port")
    ap.add_argument("--label", default="scene")
    ap.add_argument("--manifest", help="JSON list of {label, ref, port}")
    ap.add_argument("--out", default=DEFAULT_OUT)
    args = ap.parse_args()
    os.makedirs(args.out, exist_ok=True)

    pairs = []
    if args.manifest:
        with open(args.manifest) as f:
            pairs = json.load(f)
    elif args.ref and args.port:
        pairs = [{"label": args.label, "ref": args.ref, "port": args.port}]
    else:
        ap.error("provide --ref and --port, or --manifest")

    results = [compare_one(p["label"], p["ref"], p["port"], args.out) for p in pairs]
    html = write_html(results, args.out)
    with open(os.path.join(args.out, "report.json"), "w") as f:
        json.dump(results, f, indent=2)
    print(json.dumps({"ok": True, "pairs": len(results), "html": html,
                      "results": results}, indent=2))


if __name__ == "__main__":
    main()

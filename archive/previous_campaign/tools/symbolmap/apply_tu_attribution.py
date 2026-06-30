#!/usr/bin/env python3
"""apply_tu_attribution.py - fold __FILE__ string evidence into splits_refined.txt.

Updates the TU map (config/GC6E01/splits_refined.txt) from the attribution
evidence in tu_attribution.md: relabel a mislabelled range, upgrade GAP ranges
that reference a single dominant *.c literal to LIKELY, and annotate ranges that
hold a second source file (HSD split-points). Single-ref evidence inside a large
foreign range (vi.c in a DVD region, parse.c in a material region) is NOT applied
- too weak to relabel.

Edits are address-keyed (matched on the range start) and preserve column layout.
"""
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
SPLITS = ROOT / "config/GC6E01/splits_refined.txt"

# start_addr -> (new_status|None, new_src|None, note_append|None)
CORR = {
    "0x800A4D28": ("LIKELY", "src/dolphin/dvd/DVDFs.c", "dvdfs.c __FILE__ x4"),
    "0x801A69C0": ("LIKELY", "src/hsd/hsd_memory.c", "memory.c __FILE__"),
    "0x801B1730": ("LIKELY", "src/hsd/hsd_texp.c", "texp.c x15 + tev.c x5 (split)"),
    "0x801BF138": ("LIKELY", "src/hsd/hsd_video.c", "RELABEL: video.c x3 (robj.c is in the pobj range)"),
    "0x801C0000": ("LIKELY", "src/hsd/hsd_aobj.c", "aobj.c __FILE__ x3"),
    # HSD split-points: range holds a 2nd TU (annotate; boundary split is a follow-up)
    "0x80198F7C": (None, None, "+also fobj.c"),
    "0x8019B7C0": (None, None, "+also hash.c, id.c"),
    "0x8019CE50": (None, None, "+also list.c"),
    "0x801A6A34": (None, None, "+also mtx.c, perf.c"),
    "0x801AA608": (None, None, "+also robj.c"),
    "0x801BBAC8": (None, None, "+also util.c"),
}

LINE = re.compile(
    r"^(?P<status>\w+)\s+(?P<start>0x[0-9A-Fa-f]+)\s+(?P<end>0x[0-9A-Fa-f]+)\s+"
    r"(?P<src>.+?)\s+(?P<cnt>~?\d+)\s+#\s*(?P<note>.*)$")


def main() -> None:
    out, applied = [], 0
    for line in SPLITS.read_text(encoding="utf-8").splitlines():
        m = LINE.match(line)
        if m and m.group("start") in CORR:
            status, src, note = CORR[m.group("start")]
            ns = status or m.group("status")
            nsrc = src or m.group("src")
            nnote = m.group("note")
            if note:
                nnote = f"{nnote}; {note}" if nnote else note
            # Preserve approximate columns: status<8, src<40.
            line = (f"{ns:<7} {m.group('start')}  {m.group('end')}  "
                    f"{nsrc:<40} {m.group('cnt'):<5} # {nnote}")
            applied += 1
        out.append(line)
    SPLITS.write_text("\n".join(out) + "\n", encoding="utf-8")
    print(f"[tu-apply] updated {applied} ranges in splits_refined.txt")


if __name__ == "__main__":
    main()

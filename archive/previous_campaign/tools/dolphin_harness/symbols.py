"""Symbol name -> RAM address resolution for the Dolphin play harness.

Sources, in priority order:
1. symbol_aliases.json (friendly names curated for the harness)
2. config/GC6E01/symbols.txt (dtk format: `name = .section:0xADDR; // ... size:0xN ...`)
"""
import json
import os
import re

_HERE = os.path.dirname(os.path.abspath(__file__))
_REPO = os.path.dirname(os.path.dirname(_HERE))
SYMBOLS_TXT = os.path.join(_REPO, "config", "GC6E01", "symbols.txt")
ALIASES_JSON = os.path.join(_HERE, "symbol_aliases.json")

_LINE_RE = re.compile(
    r"^(?P<name>[^\s=]+)\s*=\s*\.?[\w.]+:(?P<addr>0x[0-9A-Fa-f]+);"
    r"(?:.*?size:(?P<size>0x[0-9A-Fa-f]+))?"
)

_cache = None


def _load():
    global _cache
    if _cache is not None:
        return _cache
    table = {}
    if os.path.exists(SYMBOLS_TXT):
        with open(SYMBOLS_TXT, "r", encoding="utf-8", errors="replace") as f:
            for line in f:
                m = _LINE_RE.match(line)
                if m:
                    table[m.group("name")] = {
                        "addr": int(m.group("addr"), 16),
                        "size": int(m.group("size"), 16) if m.group("size") else None,
                    }
    if os.path.exists(ALIASES_JSON):
        with open(ALIASES_JSON, "r", encoding="utf-8") as f:
            for name, info in json.load(f).items():
                table[name] = {
                    "addr": int(info["addr"], 16) if isinstance(info["addr"], str) else info["addr"],
                    "size": info.get("size"),
                }
    _cache = table
    return table


def resolve(token):
    """Resolve '0x8047...' or a symbol name. Returns (addr, size_or_None)."""
    token = token.strip()
    if re.fullmatch(r"0[xX][0-9A-Fa-f]+", token):
        return int(token, 16), None
    if re.fullmatch(r"8[0-9A-Fa-f]{7}", token):
        return int(token, 16), None
    table = _load()
    if token in table:
        return table[token]["addr"], table[token]["size"]
    # convenience: lbl_/fn_ prefix-less lookup
    for pref in ("lbl_", "fn_"):
        if pref + token in table:
            e = table[pref + token]
            return e["addr"], e["size"]
    raise KeyError(f"symbol not found: {token}")

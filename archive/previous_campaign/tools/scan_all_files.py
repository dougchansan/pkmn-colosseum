#!/usr/bin/env python3
"""Scan match% for every game .o file."""
import json, os, subprocess
from pathlib import Path
ROOT = Path(__file__).resolve().parent.parent
os.chdir(ROOT)
TARGET = str(ROOT / 'build' / 'GC6E01' / 'obj' / 'auto_01_800055E0_text.o')
CLI = str(ROOT / 'tools' / ('objdiff-cli.exe' if os.name == 'nt' else 'objdiff-cli'))

results = []
for o in sorted(os.listdir('build/GC6E01/base/game')):
    if not o.endswith('.o'): continue
    base = str(ROOT / 'build' / 'GC6E01' / 'base' / 'game' / o)
    src_c = ROOT / 'src' / 'game' / (o[:-2] + '.c')
    if not src_c.exists():
        continue
    r = subprocess.run(
        [CLI,'diff','-1',TARGET,'-2',base,'-o','-','--format','json',
         '-c','ppc.calculatePoolRelocations=false'],
        capture_output=True, text=True, shell=False
    )
    if r.returncode != 0:
        continue
    try:
        d = json.loads(r.stdout)
        for sec in d.get('right', {}).get('sections', []):
            if sec.get('name') == '.text':
                pct = sec.get('match_percent', 0)
                results.append((pct, o[:-2]))
                break
    except Exception:
        pass

results.sort()
print(f'{"%match":>8}  file')
for pct, name in results:
    print(f'{pct:7.1f}%  {name}')
print(f'\nTotal scanned: {len(results)} game .c files')

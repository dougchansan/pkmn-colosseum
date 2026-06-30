#!/usr/bin/env bash
# safe_flip_test.sh <file.c> — Flip all #if 1 functions with real C code to #if 0,
# test each one, commit matches immediately, revert failures individually.
# PREVENTS regression by committing after EACH match.

PYTHON="C:/Users/douglaswhittingham/AppData/Local/Programs/Python/Python312/python.exe"
FILE="${1:?Usage: safe_flip_test.sh <file.c>}"
MATCHED=0
FAILED=0
TOTAL=0

echo "=== Safe Flip-Test: $FILE ==="

# Get list of functions with real C code behind #if 1
FUNCTIONS=$("$PYTHON" -c "
import re
with open('$FILE', encoding='utf-8', errors='replace') as f:
    content = f.read()
pattern = r'#if 1\nasm \w+ (fn_[0-9A-Fa-f]+)\([^)]*\) \{\n#include[^\n]+\n\}\n#else\n((?:(?!#endif).)+)\n#endif'
for m in re.finditer(pattern, content, re.DOTALL):
    fn = m.group(1)
    code = m.group(2)
    if 'TODO' not in code and len(code.strip().split('\n')) > 3:
        print(fn)
")

for FN in $FUNCTIONS; do
    TOTAL=$((TOTAL + 1))
    
    # Flip this ONE function: #if 1 → #if 0
    "$PYTHON" -c "
import re
with open('$FILE', encoding='utf-8', errors='replace') as f:
    content = f.read()
pattern = r'#if 1\n(asm \w+ ${FN}\()'
m = re.search(pattern, content)
if m:
    content = content[:m.start()] + '#if 0\n' + content[m.start()+6:]
with open('$FILE', 'w', encoding='utf-8') as f:
    f.write(content)
"
    
    # Compile check
    COMPILE=$("$PYTHON" tools/compile_check.py "$FILE" 2>&1)
    if ! echo "$COMPILE" | grep -q "^OK"; then
        echo "  CFAIL $FN"
        # Revert ONLY this function
        "$PYTHON" -c "
import re
with open('$FILE', encoding='utf-8', errors='replace') as f:
    content = f.read()
pattern = r'#if 0\n(asm \w+ ${FN}\()'
m = re.search(pattern, content)
if m:
    content = content[:m.start()] + '#if 1\n' + content[m.start()+6:]
with open('$FILE', 'w', encoding='utf-8') as f:
    f.write(content)
"
        FAILED=$((FAILED + 1))
        continue
    fi
    
    # Match test
    RESULT=$("$PYTHON" tools/match_test.py "$FN" 2>&1)
    if echo "$RESULT" | grep -q "MATCHING"; then
        echo "  MATCH $FN *** 100% ***"
        # COMMIT IMMEDIATELY to prevent regression
        git add "$FILE"
        git commit -m "Phase 3: match $FN in $(basename $FILE)

Safe flip-test: existing C code verified at 100%.

Co-Authored-By: Claude Opus 4.6 (1M context) <noreply@anthropic.com>" > /dev/null 2>&1
        MATCHED=$((MATCHED + 1))
    else
        PCT=$(echo "$RESULT" | grep -oP '[\d.]+%' | head -1)
        echo "  ${PCT:-?}% $FN"
        # Revert ONLY this function
        "$PYTHON" -c "
import re
with open('$FILE', encoding='utf-8', errors='replace') as f:
    content = f.read()
pattern = r'#if 0\n(asm \w+ ${FN}\()'
m = re.search(pattern, content)
if m:
    content = content[:m.start()] + '#if 1\n' + content[m.start()+6:]
with open('$FILE', 'w', encoding='utf-8') as f:
    f.write(content)
"
        FAILED=$((FAILED + 1))
    fi
done

echo ""
echo "=== RESULTS: $MATCHED matched, $FAILED failed, $TOTAL total ==="

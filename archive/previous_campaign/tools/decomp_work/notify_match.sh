#!/usr/bin/env bash
# notify_match.sh <fn_name> — Run match_test and write result to notification file
# OpenCode should call this INSTEAD of raw match_test.py

PYTHON="C:/Users/douglaswhittingham/AppData/Local/Programs/Python/Python312/python.exe"
FN="${1:?Usage: notify_match.sh fn_XXXXXXXX}"
NOTIFY_FILE="tools/decomp_work/coordination/match_notifications.txt"
TIMESTAMP=$(date +%H:%M:%S)

# Run match test
RESULT=$("$PYTHON" tools/match_test.py "$FN" 2>&1)

# Extract match info
if echo "$RESULT" | grep -q "MATCHING"; then
    PCT="100%"
    STATUS="MATCH"
elif echo "$RESULT" | grep -q "COMPILE FAILED"; then
    PCT="FAIL"
    STATUS="COMPILE_ERROR"
else
    PCT=$(echo "$RESULT" | grep -oP '[\d.]+%' | head -1)
    STATUS="PARTIAL"
fi

INSTR=$(echo "$RESULT" | grep "Instructions:" | head -1)

# Write notification
echo "$TIMESTAMP $STATUS $FN $PCT $INSTR" >> "$NOTIFY_FILE"

# Also print for the agent to see
echo "$RESULT" | tail -6

# If match, signal loudly
if [ "$STATUS" = "MATCH" ]; then
    echo ""
    echo "=== MATCH CONFIRMED: $FN at 100% ==="
    echo "Notification written to $NOTIFY_FILE"
fi

# ALSO check if the function is actually C-active (#if 0)
IF_STATE=$(grep -B1 "asm.*${FN}" src/game/gs_title.c 2>/dev/null | head -1 | tr -d ' ')
if [ "$STATUS" = "MATCH" ] && [ "$IF_STATE" = "#if1" ]; then
    STATUS="FALSE_POSITIVE_ASM"
    echo "$TIMESTAMP FALSE_POSITIVE $FN (asm wrapper, not C code)" >> "$NOTIFY_FILE"
    echo "WARNING: $FN matched but #if 1 is active — this is the asm wrapper, not C code!"
fi

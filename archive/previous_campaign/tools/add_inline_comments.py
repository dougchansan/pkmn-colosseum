#!/usr/bin/env python3
"""Add inline comments to gs_title.c explaining CW idioms."""

# Each entry is (old_code_block, new_code_block_with_comments).
# Patterns chosen to be unique enough to match without fuzzy matching.

REPLACEMENTS = [
    # fn_800246FC + fn_80024A2C linked-list walk: the switch mode selector
    (
        "    switch ((s32)lbl_8047A370) {\n"
        "    case 1:\n"
        "        if (lbl_80478898 > lbl_8047B8D8) {\n",
        "    /* `case 0 + default` pair produces the dead `cmpwi r0, 0`    \n"
        "     * prologue CW emits before the unconditional b-to-default.   \n"
        "     * Removing `case 0:` costs 1-2% match; it must be here.      */\n"
        "    switch ((s32)lbl_8047A370) {\n"
        "    case 1:\n"
        "        if (lbl_80478898 > lbl_8047B8D8) {\n",
    ),

    # volatile u8* cast explanation (first occurrence in fn_80024BA4)
    (
        "    iVar6 = (s32)lbl_8047A36C;\n"
        "    pbVar3 = fn_8005D934(*(s16*)((u8*)fn_8005DA18(*(u32*)(arg0 + 4)) + 4));\n"
        "    iVar2 = 0;\n"
        "    while (1) {\n"
        "        if (((u32)*(volatile u8*)pbVar3 >> 7) & 1) {\n",
        "    iVar6 = (s32)lbl_8047A36C;\n"
        "    /* Nested call expr keeps intermediate ptr in r3 (volatile)   */\n"
        "    /* rather than spilling to a non-volatile save. Target does   */\n"
        "    /* the same via fn_8005D934(*(s16*)(fn_8005DA18(..) + 4))     */\n"
        "    pbVar3 = fn_8005D934(*(s16*)((u8*)fn_8005DA18(*(u32*)(arg0 + 4)) + 4));\n"
        "    iVar2 = 0;\n"
        "    while (1) {\n"
        "        /* bit 7 = \"has-data\" flag. `volatile` cast prevents CW  */\n"
        "        /* from caching the byte load in r4 across both tests -  */\n"
        "        /* target re-loads via lbz r0 each iteration.            */\n"
        "        if (((u32)*(volatile u8*)pbVar3 >> 7) & 1) {\n",
    ),

    # Second bit test (bit 6 = end-of-list)
    (
        "        if (((u32)*(volatile u8*)pbVar3 >> 6) & 1) break;\n"
        "        pbVar3 = fn_8005D934(*(s16*)(pbVar3 + 0x18));\n"
        "    }\n"
        "    pbVar3 = (u8*)0;\n"
        "LAB_80024c20:\n",
        "        /* bit 6 = \"end-of-list\" flag. If set, walk terminates.  */\n"
        "        if (((u32)*(volatile u8*)pbVar3 >> 6) & 1) break;\n"
        "        /* Follow sibling link (s16 offset at +0x18). Re-assigning */\n"
        "        /* pbVar3 from the call return lets CW chain r3 without    */\n"
        "        /* an extra `mr r29, r3` save.                             */\n"
        "        pbVar3 = fn_8005D934(*(s16*)(pbVar3 + 0x18));\n"
        "    }\n"
        "    pbVar3 = (u8*)0;\n"
        "LAB_80024c20:\n",
    ),

    # Goto LAB in for loop (fn_80024BA4 second loop)
    (
        "LAB_80024c20:\n"
        "    iVar2 = 0;\n"
        "    for (uVar7 = 0; uVar7 < *(u32*)lbl_80478DD8; uVar7++) {\n"
        "        pbVar4 = fn_8005D934(*(u32*)(lbl_80478DDC + iVar2 + 8));\n"
        "        if (pbVar3 == pbVar4) goto LAB_80024c64;\n"
        "        iVar2 = iVar2 + 0x10;\n"
        "    }\n"
        "    uVar7 = 0;\n"
        "LAB_80024c64:\n",
        "LAB_80024c20:\n"
        "    iVar2 = 0;\n"
        "    /* `goto LAB_` inside for loop emits target's `bne + b` pair. */\n"
        "    /* Using `break` here instead gives CW `beq LAB_80024c64`     */\n"
        "    /* which is semantically same but costs 1-2% match.           */\n"
        "    for (uVar7 = 0; uVar7 < *(u32*)lbl_80478DD8; uVar7++) {\n"
        "        pbVar4 = fn_8005D934(*(u32*)(lbl_80478DDC + iVar2 + 8));\n"
        "        if (pbVar3 == pbVar4) goto LAB_80024c64;\n"
        "        iVar2 = iVar2 + 0x10;\n"
        "    }\n"
        "    uVar7 = 0;    /* natural loop exit: no match; reset index    */\n"
        "LAB_80024c64:\n",
    ),

    # FP conversion: the (f32)(s32)(s16) cast chain in fn_80024DBC
    (
        "    *(s16*)(arg1 + 0x50) = (s16)(lbl_8047B8E0 + (f32)(s32)(s16)unaff_r28 + lbl_8047A374);\n",
        "    /* (f32)(s32)(s16)x forces sign-extend via extsh before the   */\n"
        "    /* xoris + stw magic f64 pattern for int-to-float conversion. */\n"
        "    /* Outer (s16) cast (not (s32)!) avoids an extra pre-store    */\n"
        "    /* extsh that target doesn't emit.                            */\n"
        "    *(s16*)(arg1 + 0x50) = (s16)(lbl_8047B8E0 + (f32)(s32)(s16)unaff_r28 + lbl_8047A374);\n",
    ),

    # fn_80022720 nested != chain
    (
        "    src = (u32*)lbl_80266C54;\n"
        "    iVar1 = 0;\n"
        "    buf[0] = src[0]; buf[1] = src[1]; buf[2] = src[2]; buf[3] = src[3]; buf[4] = src[4];\n"
        "    buf[5] = src[5]; buf[6] = src[6]; buf[7] = src[7]; buf[8] = src[8]; buf[9] = src[9];\n"
        "    if ((((arg0 != *(u16*)((u8*)buf + 0x00)) && (iVar1 = 1, arg0 != *(u16*)((u8*)buf + 0x08))) &&\n"
        "         (iVar1 = 2, arg0 != *(u16*)((u8*)buf + 0x10))) &&\n"
        "        ((iVar1 = 3, arg0 != *(u16*)((u8*)buf + 0x18)) &&\n"
        "         (iVar1 = 4, arg0 != *(u16*)((u8*)buf + 0x20)))) {\n"
        "        iVar1 = 5;\n"
        "    }\n",
        "    src = (u32*)lbl_80266C54;\n"
        "    iVar1 = 0;\n"
        "    /* Copy 10 words from static table into stack buf. Target     */\n"
        "    /* batches ALL loads then ALL stores; CW 1.3 interleaves ours */\n"
        "    /* (71% match stuck here - scheduler quirk we cannot force).  */\n"
        "    buf[0] = src[0]; buf[1] = src[1]; buf[2] = src[2]; buf[3] = src[3]; buf[4] = src[4];\n"
        "    buf[5] = src[5]; buf[6] = src[6]; buf[7] = src[7]; buf[8] = src[8]; buf[9] = src[9];\n"
        "    /* Ghidra's nested != chain with comma operator: iVar1 counts */\n"
        "    /* how many headers were tested. Each && left-operand assigns */\n"
        "    /* the next candidate index. If all 5 fail, iVar1 = 5.        */\n"
        "    /* Equivalent to: if (arg0==h[0]) iVar1=0; else if (arg0==h[1])*/\n"
        "    /* iVar1=1; ... else iVar1=5; -- but emits shorter asm.       */\n"
        "    if ((((arg0 != *(u16*)((u8*)buf + 0x00)) && (iVar1 = 1, arg0 != *(u16*)((u8*)buf + 0x08))) &&\n"
        "         (iVar1 = 2, arg0 != *(u16*)((u8*)buf + 0x10))) &&\n"
        "        ((iVar1 = 3, arg0 != *(u16*)((u8*)buf + 0x18)) &&\n"
        "         (iVar1 = 4, arg0 != *(u16*)((u8*)buf + 0x20)))) {\n"
        "        iVar1 = 5;\n"
        "    }\n",
    ),

    # fn_80024CDC clamp
    (
        "    bVar1 = *(u8*)(arg1 + 0x67);\n"
        "    uVar3 = fn_800D3088();\n"
        "    fVar2 = lbl_8047A37C;\n"
        "    iVar4 = (s32)(fVar2 * (f32)(u32)uVar3 + (f32)(s32)bVar1);\n"
        "\n"
        "    if (iVar4 < 0x40) {\n"
        "        iVar4 = 0x40;\n"
        "        lbl_8047A37C = -fVar2;\n"
        "    } else if (0xFF < iVar4) {\n"
        "        iVar4 = 0xFF;\n"
        "        lbl_8047A37C = -fVar2;\n"
        "    }\n",
        "    bVar1 = *(u8*)(arg1 + 0x67);          /* current alpha (0..255)    */\n"
        "    uVar3 = fn_800D3088();                /* u32 tick counter          */\n"
        "    fVar2 = lbl_8047A37C;                 /* f32 pulse speed           */\n"
        "    /* (f32)(u32)uVar3 -> unsigned int-to-float magic (no xor).  */\n"
        "    /* (f32)(s32)bVar1 -> signed int-to-float (xoris + stw).     */\n"
        "    iVar4 = (s32)(fVar2 * (f32)(u32)uVar3 + (f32)(s32)bVar1);\n"
        "\n"
        "    /* Clamp alpha to [0x40, 0xFF] and flip direction on rail.   */\n"
        "    /* This produces the classic ping-pong / throb animation.   */\n"
        "    if (iVar4 < 0x40) {\n"
        "        iVar4 = 0x40;\n"
        "        lbl_8047A37C = -fVar2;            /* reverse pulse direction   */\n"
        "    } else if (0xFF < iVar4) {\n"
        "        iVar4 = 0xFF;\n"
        "        lbl_8047A37C = -fVar2;\n"
        "    }\n",
    ),
]

with open('src/game/gs_title.c', 'rb') as f:
    content = f.read().decode('utf-8')

applied = 0
for old, new in REPLACEMENTS:
    if old in content:
        content = content.replace(old, new, 1)
        applied += 1
        print(f"Applied replacement #{applied}")
    else:
        print(f"SKIP (pattern not found): {old[:60]!r}")

with open('src/game/gs_title.c', 'wb') as f:
    f.write(content.encode('utf-8'))

print(f"Done. {applied}/{len(REPLACEMENTS)} replacements applied.")

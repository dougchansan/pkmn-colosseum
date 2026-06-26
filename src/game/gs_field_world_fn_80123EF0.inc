    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stmw r26, 0x8(r1)
    mr. r26, r3
    mr r27, r5
    mr r28, r6
    mr r29, r7
    mr r30, r8
    mr r31, r9
    beq @L_80123FA8
    mr r7, r4
    li r4, 0x0
    li r5, 0x71
    li r6, 0x0
    bl fn_801254B4
    mr r3, r26
    clrlwi r7, r27, 24
    li r4, 0x0
    li r5, 0x72
    li r6, 0x0
    bl fn_801254B4
    mr r3, r26
    clrlwi r7, r28, 16
    li r4, 0x0
    li r5, 0x73
    li r6, 0x0
    bl fn_801254B4
    mr r3, r26
    clrlwi r7, r29, 24
    li r4, 0x0
    li r5, 0x74
    li r6, 0x0
    bl fn_801254B4
    mr r3, r26
    mr r7, r30
    li r4, 0x0
    li r5, 0x75
    li r6, 0x0
    bl fn_801254B4
    mr r3, r26
    mr r7, r31
    li r4, 0x0
    li r5, 0x76
    li r6, 0x0
    bl fn_801254B4
@L_80123FA8:
    lmw r26, 0x8(r1)
    lwz r0, 0x24(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr

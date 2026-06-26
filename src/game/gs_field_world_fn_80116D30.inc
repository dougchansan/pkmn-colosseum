    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stmw r26, 0x8(r1)
    mr r26, r3
    mr r29, r4
    lis r5, lbl_80272708@ha
    addi r31, r5, lbl_80272708@l
    bl fn_801141F8
    extsb r28, r26
    cmpwi r28, 0x3
    beq @L_80116DA8
    bge @L_80116D74
    cmpwi r28, 0x1
    beq @L_80116D80
    bge @L_80116D94
    b @L_80116DCC
@L_80116D74:
    cmpwi r28, 0x5
    bge @L_80116DCC
    b @L_80116DBC
@L_80116D80:
    mr r4, r29
    addi r3, r31, 0x14
    crxor 6, 6, 6
    bl fn_800DD970
    b @L_80116DCC
@L_80116D94:
    mr r4, r29
    addi r3, r31, 0x28
    crxor 6, 6, 6
    bl fn_800DD970
    b @L_80116DCC
@L_80116DA8:
    mr r4, r29
    addi r3, r31, 0x3c
    crxor 6, 6, 6
    bl fn_800DD970
    b @L_80116DCC
@L_80116DBC:
    mr r4, r29
    addi r3, r31, 0x50
    crxor 6, 6, 6
    bl fn_800DD970
@L_80116DCC:
    bl fn_800FF56C
    mr r31, r3
    li r26, 0x0
    li r30, 0x0
    b @L_80116E48
@L_80116DE0:
    lwz r0, lbl_80478EC4(r13)
    add r27, r0, r30
    lhz r0, 0x2(r27)
    cmplw r0, r31
    bne @L_80116E40
    lbz r0, 0x0(r27)
    cmpw r0, r28
    bne @L_80116E40
    lwz r0, 0x4(r27)
    cmplw r0, r29
    bne @L_80116E40
    lwz r0, 0x8(r27)
    cmplwi r0, 0x0
    beq @L_80116E40
    mr r3, r29
    bl fn_801CA9F8
    lwz r3, 0x8(r27)
    li r4, 0x4
    lwz r5, 0xc(r27)
    lwz r6, 0x10(r27)
    lwz r7, 0x14(r27)
    lwz r8, 0x18(r27)
    crxor 6, 6, 6
    bl fn_800F7434
@L_80116E40:
    addi r30, r30, 0x1c
    addi r26, r26, 0x1
@L_80116E48:
    lwz r3, lbl_80478EC0(r13)
    lwz r0, 0x0(r3)
    cmplw r26, r0
    blt @L_80116DE0
    lmw r26, 0x8(r1)
    lwz r0, 0x24(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr

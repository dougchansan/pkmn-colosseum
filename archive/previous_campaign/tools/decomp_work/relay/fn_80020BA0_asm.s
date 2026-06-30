    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    stw r30, 0x8(r1)
    mr r30, r3
    mr r31, r4
    bl fn_80166C74
    cmplwi r3, 0x0
    bne @L_80020BDC
    lis r3, lbl_802EF0A8@ha
    addi r3, r3, lbl_802EF0A8@l
    addis r9, r3, 0x1
    subi r9, r9, 0x7010
    b @L_80020BEC
@L_80020BDC:
    lis r3, lbl_802EF0A8@ha
    addi r3, r3, lbl_802EF0A8@l
    addis r9, r3, 0x1
    subi r9, r9, 0x6ff4
@L_80020BEC:
    lha r8, 0x50(r31)
    mr r5, r30
    lha r3, 0x2(r9)
    li r6, 0x192
    lha r4, 0x52(r31)
    li r7, 0x0
    lha r0, 0x4(r9)
    subf r3, r8, r3
    extsh r3, r3
    subf r0, r4, r0
    extsh r4, r0
    bl fn_801040F0
    li r3, 0x0
    li r4, 0x9
    bl fn_80135168
    cmpwi r3, 0x1
    bne @L_80020C44
    lis r3, lbl_802EF0A8@ha
    addi r3, r3, lbl_802EF0A8@l
    addis r9, r3, 0x1
    subi r9, r9, 0x702c
    b @L_80020C54
@L_80020C44:
    lis r3, lbl_802EF0A8@ha
    addi r3, r3, lbl_802EF0A8@l
    addis r9, r3, 0x1
    subi r9, r9, 0x7048
@L_80020C54:
    lha r8, 0x50(r31)
    mr r5, r30
    lha r3, 0x2(r9)
    li r6, 0x192
    lha r4, 0x52(r31)
    li r7, 0x0
    lha r0, 0x4(r9)
    subf r3, r8, r3
    extsh r3, r3
    subf r0, r4, r0
    extsh r4, r0
    bl fn_801040F0
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    lwz r30, 0x8(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr


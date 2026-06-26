    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    stw r30, 0x8(r1)
    mr r30, r5
    addi r0, r30, 0x1f
    mr r5, r3
    mr r6, r4
    li r4, 0x20
    clrrwi r3, r0, 5
    li r7, 0x0
    bl fn_800F9418
    mr r31, r3
    cmplwi r31, 0x0
    bne @L_80115078
    lis r3, lbl_802725CC@ha
    mr r4, r30
    addi r3, r3, lbl_802725CC@l
    crxor 6, 6, 6
    bl fn_800DD970
@L_80115078:
    mr r3, r31
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    lwz r30, 0x8(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr

    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    cmplwi r3, 0x0
    bne @L_801156B8
    lis r3, lbl_80272608@ha
    lis r4, lbl_8035BA60@ha
    addi r3, r3, lbl_80272608@l
    addi r4, r4, lbl_8035BA60@l
    crxor 6, 6, 6
    bl fn_800DD970
    li r3, 0x0
    b @L_801156F4
@L_801156B8:
    lwz r3, 0x14(r3)
    cmplwi r3, 0x0
    bne @L_801156CC
    li r3, 0x0
    b @L_801156F4
@L_801156CC:
    lwz r5, 0x0(r3)
    lwz r3, 0x0(r5)
    lwz r0, 0x0(r3)
    cmplw r4, r0
    blt @L_801156E8
    li r3, 0x0
    b @L_801156F4
@L_801156E8:
    mulli r0, r4, 0x24
    lwz r3, 0x4(r5)
    add r3, r3, r0
@L_801156F4:
    lwz r0, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr

    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    cmplwi r3, 0x0
    bne @L_801159DC
    lis r3, lbl_80272608@ha
    lis r4, lbl_8035B950@ha
    addi r3, r3, lbl_80272608@l
    addi r4, r4, lbl_8035B950@l
    crxor 6, 6, 6
    bl fn_800DD970
    li r3, 0x0
    b @L_801159E0
@L_801159DC:
    lwz r3, 0x28(r3)
@L_801159E0:
    lwz r0, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr

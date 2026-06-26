    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    mr r31, r3
    cmplwi r31, 0x0
    bne @L_80115B04
    lis r3, lbl_80272608@ha
    lis r4, lbl_8035B8E8@ha
    addi r3, r3, lbl_80272608@l
    addi r4, r4, lbl_8035B8E8@l
    crxor 6, 6, 6
    bl fn_800DD970
    li r3, 0x0
    b @L_80115B34
@L_80115B04:
    lbz r0, 0x0(r31)
    extrwi r0, r0, 3, 24
    cmplwi r0, 0x2
    bne @L_80115B2C
    bl fn_801EF624
    clrlwi r0, r3, 16
    cmplwi r0, 0x0
    bne @L_80115B2C
    li r3, 0x1
    b @L_80115B34
@L_80115B2C:
    lbz r0, 0x0(r31)
    extrwi r3, r0, 3, 24
@L_80115B34:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr

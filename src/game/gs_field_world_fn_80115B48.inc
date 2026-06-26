    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    cmplwi r3, 0x0
    bne @L_80115B7C
    lis r3, lbl_802726AC@ha
    lis r4, lbl_8035B8CC@ha
    addi r3, r3, lbl_802726AC@l
    addi r4, r4, lbl_8035B8CC@l
    crxor 6, 6, 6
    bl fn_800DD970
    li r3, 0x0
    b @L_80115B80
@L_80115B7C:
    lwz r3, 0x8(r3)
@L_80115B80:
    lwz r0, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr

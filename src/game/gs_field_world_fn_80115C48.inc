    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    lwz r4, lbl_80478FB8(r13)
    lwz r5, lbl_80478FBC(r13)
    lwz r0, 0x0(r4)
    mtctr r0
    cmplwi r0, 0x0
    beq @L_80115C88
@L_80115C6C:
    lwz r0, 0xc(r5)
    cmplw r0, r3
    bne @L_80115C80
    mr r3, r5
    b @L_80115CA4
@L_80115C80:
    addi r5, r5, 0x4c
    bdnz @L_80115C6C
@L_80115C88:
    lis r3, lbl_802726D4@ha
    lis r4, lbl_8035B8A0@ha
    addi r3, r3, lbl_802726D4@l
    addi r4, r4, lbl_8035B8A0@l
    crxor 6, 6, 6
    bl fn_800DD970
    li r3, 0x0
@L_80115CA4:
    lwz r0, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr

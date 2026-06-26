    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    cmplwi r3, 0x0
    bne @L_80115570
    lis r3, lbl_80272658@ha
    lis r6, lbl_8035BAD8@ha
    mr r5, r4
    addi r3, r3, lbl_80272658@l
    addi r4, r6, lbl_8035BAD8@l
    crxor 6, 6, 6
    bl fn_800DD970
    b @L_80115574
@L_80115570:
    stw r4, 0x34(r3)
@L_80115574:
    lwz r0, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr

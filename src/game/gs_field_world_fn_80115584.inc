    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    cmplwi r3, 0x0
    bne @L_801155B8
    lis r3, lbl_80272680@ha
    lis r6, lbl_8035BABC@ha
    mr r5, r4
    addi r3, r3, lbl_80272680@l
    addi r4, r6, lbl_8035BABC@l
    crxor 6, 6, 6
    bl fn_800DD970
    b @L_801155BC
@L_801155B8:
    stw r4, 0x8(r3)
@L_801155BC:
    lwz r0, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr

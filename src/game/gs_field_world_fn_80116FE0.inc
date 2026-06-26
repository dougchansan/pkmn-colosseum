    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    mr r5, r3
    cmplwi r5, 0x0
    bne @L_80117000
    li r3, 0x0
    b @L_80117028
@L_80117000:
    cmplwi r4, 0x0
    bne @L_80117010
    li r3, 0x0
    b @L_80117028
@L_80117010:
    lfs f1, 0x18(r5)
    mr r3, r4
    lfs f2, 0x1c(r5)
    lfs f3, 0x20(r5)
    bl fn_800E01F4
    li r3, 0x1
@L_80117028:
    lwz r0, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr

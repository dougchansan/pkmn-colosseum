    nofralloc
    cmplwi r3, 0x0
    beqlr
    cmplwi r4, 0x0
    beqlr
    li r0, 0x163
    subi r5, r3, 0x4
    subi r4, r4, 0x4
    mtctr r0
@L_8012AC84:
    lwz r3, 0x4(r4)
    lwzu r0, 0x8(r4)
    stw r3, 0x4(r5)
    stwu r0, 0x8(r5)
    bdnz @L_8012AC84
    blr

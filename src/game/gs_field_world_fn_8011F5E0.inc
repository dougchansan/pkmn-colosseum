    nofralloc
    cmplwi r3, 0x0
    beqlr
    cmplwi r4, 0x0
    beqlr
    lwz r0, 0x0(r4)
    stw r0, 0x0(r3)
    blr

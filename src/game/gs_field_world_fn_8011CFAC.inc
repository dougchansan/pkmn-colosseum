    nofralloc
    cmplwi r3, 0x0
    beqlr
    stw r4, 0x20(r3)
    blr

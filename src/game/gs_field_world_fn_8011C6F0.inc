    nofralloc
    cmplwi r3, 0x0
    beqlr
    stw r4, 0x28(r3)
    blr

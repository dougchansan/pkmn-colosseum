    nofralloc
    cmplwi r3, 0x0
    beqlr
    stw r4, 0xe0(r3)
    blr

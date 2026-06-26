    nofralloc
    cmplwi r3, 0x0
    beqlr
    stw r4, 0x1c(r3)
    blr

    nofralloc
    cmplwi r3, 0x0
    beqlr
    sth r4, 0xe(r3)
    blr

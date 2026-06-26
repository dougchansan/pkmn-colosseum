    nofralloc
    cmplwi r3, 0x0
    beqlr
    rlwinm r0, r4, 0, 24, 31
    stw r0, 0xf8(r3)
    blr

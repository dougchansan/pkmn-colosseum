    nofralloc
    cmplwi r3, 0x0
    beqlr
    rlwinm r0, r4, 0, 16, 31
    cmplwi r0, 0x3
    bgelr
    add r3, r3, r0
    stb r5, 0x34(r3)
    blr

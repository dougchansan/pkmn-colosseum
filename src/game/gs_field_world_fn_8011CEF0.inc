    nofralloc
    cmplwi r3, 0x0
    beqlr
    rlwinm r0, r4, 0, 16, 31
    cmplwi r0, 0x8
    bgelr
    clrlslwi r0, r4, 16, 1
    add r3, r3, r0
    sth r5, 0x74(r3)
    blr

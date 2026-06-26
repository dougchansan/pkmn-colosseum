    nofralloc
    cmplwi r3, 0x0
    beqlr
    cmplwi r4, 0x0
    beqlr
    lfs f0, 0x0(r4)
    stfs f0, 0x18(r3)
    lfs f0, 0x4(r4)
    stfs f0, 0x1c(r3)
    lfs f0, 0x8(r4)
    stfs f0, 0x20(r3)
    blr

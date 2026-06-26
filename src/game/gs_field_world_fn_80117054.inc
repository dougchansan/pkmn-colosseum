    nofralloc
    cmplwi r3, 0x0
    beq @L_80117068
    lbz r0, 0x0(r3)
    rlwinm r3, r0, 25, 31, 31
    blr
@L_80117068:
    li r3, 0x0
    blr

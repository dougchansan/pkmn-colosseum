    nofralloc
    cmplwi r3, 0x0
    beq @L_80116F44
    lbz r0, 0x1(r3)
    rlwinm r3, r0, 26, 30, 31
    blr
@L_80116F44:
    li r3, 0x0
    blr

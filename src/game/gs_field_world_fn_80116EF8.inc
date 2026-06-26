    nofralloc
    cmplwi r3, 0x0
    beq @L_80116F0C
    lbz r0, 0x0(r3)
    rlwinm r3, r0, 28, 31, 31
    blr
@L_80116F0C:
    li r3, 0x0
    blr

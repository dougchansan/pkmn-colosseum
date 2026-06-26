    nofralloc
    cmplwi r3, 0x0
    beq @L_80116F60
    lbz r0, 0x0(r3)
    rlwinm r3, r0, 0, 29, 31
    blr
@L_80116F60:
    li r3, 0x0
    blr

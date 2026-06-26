    nofralloc
    cmplwi r3, 0x0
    beq @L_80116EF0
    lhz r3, 0x8(r3)
    blr
@L_80116EF0:
    li r3, 0x0
    blr

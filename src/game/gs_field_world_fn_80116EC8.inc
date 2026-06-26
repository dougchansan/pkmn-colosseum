    nofralloc
    cmplwi r3, 0x0
    beq @L_80116ED8
    lwz r3, 0x10(r3)
    blr
@L_80116ED8:
    li r3, 0x0
    blr

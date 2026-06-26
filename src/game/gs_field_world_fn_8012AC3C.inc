    nofralloc
    cmplwi r3, 0x0
    bne @L_8012AC4C
    li r3, 0x0
    blr
@L_8012AC4C:
    lwz r3, 0x2c(r3)
    blr

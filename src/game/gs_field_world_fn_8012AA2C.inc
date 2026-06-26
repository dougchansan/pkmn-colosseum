    nofralloc
    cmplwi r3, 0x0
    bne @L_8012AA3C
    li r3, 0x0
    blr
@L_8012AA3C:
    lbz r3, 0xa80(r3)
    blr

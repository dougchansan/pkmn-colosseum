    nofralloc
    cmplwi r3, 0x0
    bne @L_8012A81C
    li r3, 0x0
    blr
@L_8012A81C:
    lwz r3, 0xa88(r3)
    blr

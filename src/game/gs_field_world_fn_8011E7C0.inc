    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E7D0
    li r3, 0x0
    blr
@L_8011E7D0:
    lhz r3, 0xd6(r3)
    blr

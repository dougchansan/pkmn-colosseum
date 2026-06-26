    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E040
    li r3, 0x0
    blr
@L_8011E040:
    lhz r3, 0x16(r3)
    blr

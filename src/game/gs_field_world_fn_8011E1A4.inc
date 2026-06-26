    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E1B4
    li r3, 0x0
    blr
@L_8011E1B4:
    lhz r3, 0xa(r3)
    blr

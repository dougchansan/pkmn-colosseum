    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E548
    li r3, 0x0
    blr
@L_8011E548:
    lhz r3, 0x6(r3)
    blr

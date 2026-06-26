    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E16C
    li r3, 0x0
    blr
@L_8011E16C:
    lhz r3, 0xe(r3)
    blr

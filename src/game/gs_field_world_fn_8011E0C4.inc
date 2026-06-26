    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E0D4
    li r3, 0x0
    blr
@L_8011E0D4:
    lhz r3, 0x12(r3)
    blr

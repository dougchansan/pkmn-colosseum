    nofralloc
    cmplwi r3, 0x0
    bne @L_8011F590
    li r3, 0x0
    blr
@L_8011F590:
    lhz r3, 0xc(r3)
    blr

    nofralloc
    cmplwi r3, 0x0
    bne @L_8011F5D8
    li r3, 0x0
    blr
@L_8011F5D8:
    lhz r3, 0x0(r3)
    blr

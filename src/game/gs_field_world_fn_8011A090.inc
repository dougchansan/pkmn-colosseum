    nofralloc
    cmplwi r3, 0x0
    bne @L_8011A0A0
    li r3, 0x0
    blr
@L_8011A0A0:
    lhz r3, 0x0(r3)
    blr

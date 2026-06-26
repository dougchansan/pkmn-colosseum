    nofralloc
    cmplwi r3, 0x0
    bne @L_8011F1B0
    li r3, 0x0
    blr
@L_8011F1B0:
    lhz r3, 0x88(r3)
    blr

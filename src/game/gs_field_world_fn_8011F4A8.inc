    nofralloc
    cmplwi r3, 0x0
    bne @L_8011F4B8
    li r3, 0x0
    blr
@L_8011F4B8:
    lbz r3, 0x60(r3)
    blr

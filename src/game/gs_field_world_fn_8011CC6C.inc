    nofralloc
    cmplwi r3, 0x0
    bne @L_8011CC7C
    li r3, 0x0
    blr
@L_8011CC7C:
    lbz r3, 0x10(r3)
    blr

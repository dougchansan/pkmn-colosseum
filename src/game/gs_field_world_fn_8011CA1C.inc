    nofralloc
    cmplwi r3, 0x0
    bne @L_8011CA2C
    li r3, 0x0
    blr
@L_8011CA2C:
    lwz r3, 0x20(r3)
    blr

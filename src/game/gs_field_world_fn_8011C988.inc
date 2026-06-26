    nofralloc
    cmplwi r3, 0x0
    bne @L_8011C998
    li r3, 0x0
    blr
@L_8011C998:
    lha r3, 0x16(r3)
    blr

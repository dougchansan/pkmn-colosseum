    nofralloc
    cmplwi r3, 0x0
    bne @L_8011C980
    li r3, 0x0
    blr
@L_8011C980:
    lhz r3, 0x18(r3)
    blr

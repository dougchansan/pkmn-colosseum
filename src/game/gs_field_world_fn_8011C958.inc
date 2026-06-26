    nofralloc
    cmplwi r3, 0x0
    bne @L_8011C968
    li r3, 0x0
    blr
@L_8011C968:
    lhz r3, 0x1a(r3)
    blr

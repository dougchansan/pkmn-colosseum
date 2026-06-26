    nofralloc
    cmplwi r3, 0x0
    bne @L_8011C7D0
    li r3, 0x0
    blr
@L_8011C7D0:
    lwz r3, 0x2c(r3)
    blr

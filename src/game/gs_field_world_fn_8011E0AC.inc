    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E0BC
    li r3, 0x0
    blr
@L_8011E0BC:
    lwz r3, 0x2c(r3)
    blr

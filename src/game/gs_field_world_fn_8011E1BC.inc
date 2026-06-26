    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E1CC
    li r3, 0x0
    blr
@L_8011E1CC:
    lwz r3, 0x1c(r3)
    blr

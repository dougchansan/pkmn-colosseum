    nofralloc
    cmplwi r3, 0x0
    bne @L_8011F4D0
    li r3, 0x0
    blr
@L_8011F4D0:
    lwz r3, 0x5c(r3)
    blr

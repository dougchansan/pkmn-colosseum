    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E184
    li r3, 0x0
    blr
@L_8011E184:
    lwz r3, 0x20(r3)
    blr

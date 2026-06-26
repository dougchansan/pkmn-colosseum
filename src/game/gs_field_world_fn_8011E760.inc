    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E770
    li r3, 0x0
    blr
@L_8011E770:
    lwz r3, 0x18(r3)
    blr

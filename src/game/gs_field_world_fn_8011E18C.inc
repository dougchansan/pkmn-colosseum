    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E19C
    li r3, 0x0
    blr
@L_8011E19C:
    lhz r3, 0xc(r3)
    blr

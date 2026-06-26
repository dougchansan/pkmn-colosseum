    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E7E8
    li r3, 0x0
    blr
@L_8011E7E8:
    lhz r3, 0xd2(r3)
    blr

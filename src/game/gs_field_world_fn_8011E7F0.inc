    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E800
    li r3, 0x0
    blr
@L_8011E800:
    lhz r3, 0xd4(r3)
    blr

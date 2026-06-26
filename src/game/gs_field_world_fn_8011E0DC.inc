    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E0EC
    li r3, 0x0
    blr
@L_8011E0EC:
    lhz r3, 0x14(r3)
    blr

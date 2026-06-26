    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E8EC
    li r3, 0x0
    blr
@L_8011E8EC:
    lbz r3, 0xcb(r3)
    blr

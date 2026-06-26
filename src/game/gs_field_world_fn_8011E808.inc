    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E818
    li r3, 0x0
    blr
@L_8011E818:
    lbz r3, 0xd0(r3)
    blr

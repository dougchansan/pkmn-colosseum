    nofralloc
    cmplwi r3, 0x0
    bne @L_8011CD44
    li r3, 0x0
    blr
@L_8011CD44:
    lbz r0, 0x2(r3)
    extsb r3, r0
    blr

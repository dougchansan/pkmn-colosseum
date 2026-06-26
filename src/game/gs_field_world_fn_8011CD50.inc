    nofralloc
    cmplwi r3, 0x0
    bne @L_8011CD60
    li r3, 0x0
    blr
@L_8011CD60:
    lbz r0, 0x1(r3)
    extsb r3, r0
    blr

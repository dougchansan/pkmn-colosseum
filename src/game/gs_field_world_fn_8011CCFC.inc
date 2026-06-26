    nofralloc
    cmplwi r3, 0x0
    bne @L_8011CD0C
    li r3, 0x0
    blr
@L_8011CD0C:
    lbz r0, 0x4(r3)
    extsb r3, r0
    blr

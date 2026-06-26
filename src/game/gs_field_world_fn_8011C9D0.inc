    nofralloc
    cmplwi r3, 0x0
    bne @L_8011C9E0
    li r3, 0x0
    blr
@L_8011C9E0:
    lbz r0, 0x0(r3)
    extsb r3, r0
    blr

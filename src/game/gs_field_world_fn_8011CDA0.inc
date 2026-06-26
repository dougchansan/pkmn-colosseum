    nofralloc
    cmplwi r3, 0x0
    bne @L_8011CDB0
    li r3, 0x0
    blr
@L_8011CDB0:
    lbz r3, 0x8(r3)
    blr

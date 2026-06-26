    nofralloc
    cmplwi r3, 0x0
    bne @L_8011C848
    li r3, 0x0
    blr
@L_8011C848:
    lbz r3, 0xe(r3)
    blr

    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D384
    li r3, 0x0
    b @L_8011D388
@L_8011D384:
    addi r3, r3, 0x90
@L_8011D388:
    cmplwi r3, 0x0
    beqlr
    sth r4, 0x0(r3)
    blr

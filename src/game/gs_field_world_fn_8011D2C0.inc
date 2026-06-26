    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D2D0
    li r3, 0x0
    b @L_8011D2D4
@L_8011D2D0:
    addi r3, r3, 0x90
@L_8011D2D4:
    cmplwi r3, 0x0
    beqlr
    sth r4, 0xa(r3)
    blr

    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D360
    li r3, 0x0
    b @L_8011D364
@L_8011D360:
    addi r3, r3, 0x90
@L_8011D364:
    cmplwi r3, 0x0
    beqlr
    sth r4, 0x2(r3)
    blr

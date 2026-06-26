    nofralloc
    cmplwi r3, 0x0
    bne @L_8011DC10
    li r3, 0x0
    b @L_8011DC14
@L_8011DC10:
    addi r3, r3, 0x8c
@L_8011DC14:
    cmplwi r3, 0x0
    beqlr
    sth r4, 0x4(r3)
    blr

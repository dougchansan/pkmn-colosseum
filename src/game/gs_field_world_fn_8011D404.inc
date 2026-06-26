    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D414
    li r3, 0x0
    b @L_8011D418
@L_8011D414:
    addi r3, r3, 0x84
@L_8011D418:
    cmplwi r3, 0x0
    beqlr
    sth r4, 0x4(r3)
    blr

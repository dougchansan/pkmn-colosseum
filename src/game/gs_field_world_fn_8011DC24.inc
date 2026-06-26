    nofralloc
    cmplwi r3, 0x0
    bne @L_8011DC34
    li r3, 0x0
    b @L_8011DC38
@L_8011DC34:
    addi r3, r3, 0x8c
@L_8011DC38:
    cmplwi r3, 0x0
    beqlr
    sth r4, 0x2(r3)
    blr

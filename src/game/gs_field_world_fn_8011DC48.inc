    nofralloc
    cmplwi r3, 0x0
    bne @L_8011DC58
    li r3, 0x0
    b @L_8011DC5C
@L_8011DC58:
    addi r3, r3, 0x8c
@L_8011DC5C:
    cmplwi r3, 0x0
    beqlr
    sth r4, 0x0(r3)
    blr

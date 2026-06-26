    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D2F4
    li r3, 0x0
    b @L_8011D2F8
@L_8011D2F4:
    addi r3, r3, 0x90
@L_8011D2F8:
    cmplwi r3, 0x0
    beqlr
    sth r4, 0x8(r3)
    blr

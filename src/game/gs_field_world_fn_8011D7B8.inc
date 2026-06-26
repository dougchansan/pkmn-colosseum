    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D7C8
    li r3, 0x0
    b @L_8011D7CC
@L_8011D7C8:
    addi r3, r3, 0xb7
@L_8011D7CC:
    cmplwi r3, 0x0
    beqlr
    stb r4, 0x2(r3)
    blr

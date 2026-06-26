    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D7A4
    li r3, 0x0
    b @L_8011D7A8
@L_8011D7A4:
    addi r3, r3, 0xb7
@L_8011D7A8:
    cmplwi r3, 0x0
    beqlr
    stb r4, 0x3(r3)
    blr

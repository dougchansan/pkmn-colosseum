    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D62C
    li r3, 0x0
    b @L_8011D630
@L_8011D62C:
    addi r3, r3, 0xbd
@L_8011D630:
    cmplwi r3, 0x0
    beqlr
    stb r4, 0x8(r3)
    blr

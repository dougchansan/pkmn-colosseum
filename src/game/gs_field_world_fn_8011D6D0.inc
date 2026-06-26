    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D6E0
    li r3, 0x0
    b @L_8011D6E4
@L_8011D6E0:
    addi r3, r3, 0xbd
@L_8011D6E4:
    cmplwi r3, 0x0
    beqlr
    stb r4, 0x3(r3)
    blr

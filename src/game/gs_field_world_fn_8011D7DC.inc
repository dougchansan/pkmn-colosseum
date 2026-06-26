    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D7EC
    li r3, 0x0
    b @L_8011D7F0
@L_8011D7EC:
    addi r3, r3, 0xb7
@L_8011D7F0:
    cmplwi r3, 0x0
    beqlr
    stb r4, 0x1(r3)
    blr

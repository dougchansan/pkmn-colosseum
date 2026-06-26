    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D8C4
    li r3, 0x0
    b @L_8011D8C8
@L_8011D8C4:
    addi r3, r3, 0xb2
@L_8011D8C8:
    cmplwi r3, 0x0
    beqlr
    stb r4, 0x0(r3)
    blr

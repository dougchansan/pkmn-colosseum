    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D8A0
    li r3, 0x0
    b @L_8011D8A4
@L_8011D8A0:
    addi r3, r3, 0xb2
@L_8011D8A4:
    cmplwi r3, 0x0
    beqlr
    stb r4, 0x1(r3)
    blr

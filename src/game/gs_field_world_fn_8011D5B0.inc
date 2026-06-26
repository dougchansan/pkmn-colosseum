    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D5C0
    li r3, 0x0
    b @L_8011D5C4
@L_8011D5C0:
    addi r3, r3, 0xbd
@L_8011D5C4:
    cmplwi r3, 0x0
    beqlr
    stb r4, 0xb(r3)
    blr

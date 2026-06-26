    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D5E4
    li r3, 0x0
    b @L_8011D5E8
@L_8011D5E4:
    addi r3, r3, 0xbd
@L_8011D5E8:
    cmplwi r3, 0x0
    beqlr
    stb r4, 0xa(r3)
    blr

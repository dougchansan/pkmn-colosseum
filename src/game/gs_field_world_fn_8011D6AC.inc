    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D6BC
    li r3, 0x0
    b @L_8011D6C0
@L_8011D6BC:
    addi r3, r3, 0xbd
@L_8011D6C0:
    cmplwi r3, 0x0
    beqlr
    stb r4, 0x4(r3)
    blr

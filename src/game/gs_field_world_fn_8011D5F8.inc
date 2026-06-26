    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D608
    li r3, 0x0
    b @L_8011D60C
@L_8011D608:
    addi r3, r3, 0xbd
@L_8011D60C:
    cmplwi r3, 0x0
    beqlr
    stb r4, 0x9(r3)
    blr

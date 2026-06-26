    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D728
    li r3, 0x0
    b @L_8011D72C
@L_8011D728:
    addi r3, r3, 0xbd
@L_8011D72C:
    cmplwi r3, 0x0
    beqlr
    stb r4, 0x1(r3)
    blr

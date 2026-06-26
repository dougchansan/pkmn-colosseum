    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D59C
    li r3, 0x0
    b @L_8011D5A0
@L_8011D59C:
    addi r3, r3, 0xbd
@L_8011D5A0:
    cmplwi r3, 0x0
    beqlr
    stb r4, 0xc(r3)
    blr

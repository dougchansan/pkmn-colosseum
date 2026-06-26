    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D698
    li r3, 0x0
    b @L_8011D69C
@L_8011D698:
    addi r3, r3, 0xbd
@L_8011D69C:
    cmplwi r3, 0x0
    beqlr
    stb r4, 0x5(r3)
    blr

    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D810
    li r3, 0x0
    b @L_8011D814
@L_8011D810:
    addi r3, r3, 0xb7
@L_8011D814:
    cmplwi r3, 0x0
    beqlr
    stb r4, 0x0(r3)
    blr

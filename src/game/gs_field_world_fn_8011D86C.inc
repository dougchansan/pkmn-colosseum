    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D87C
    li r3, 0x0
    b @L_8011D880
@L_8011D87C:
    addi r3, r3, 0xb2
@L_8011D880:
    cmplwi r3, 0x0
    beqlr
    stb r4, 0x2(r3)
    blr

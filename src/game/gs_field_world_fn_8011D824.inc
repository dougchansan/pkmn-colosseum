    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D834
    li r3, 0x0
    b @L_8011D838
@L_8011D834:
    addi r3, r3, 0xb2
@L_8011D838:
    cmplwi r3, 0x0
    beqlr
    stb r4, 0x4(r3)
    blr

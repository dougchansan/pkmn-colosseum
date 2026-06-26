    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D3CC
    li r3, 0x0
    b @L_8011D3D0
@L_8011D3CC:
    addi r3, r3, 0x84
@L_8011D3D0:
    cmplwi r3, 0x0
    beqlr
    sth r4, 0x8(r3)
    blr

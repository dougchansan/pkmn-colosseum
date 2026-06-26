    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D45C
    li r3, 0x0
    b @L_8011D460
@L_8011D45C:
    addi r3, r3, 0x84
@L_8011D460:
    cmplwi r3, 0x0
    beqlr
    sth r4, 0x0(r3)
    blr

    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D9D0
    li r3, 0x0
    b @L_8011D9D4
@L_8011D9D0:
    addi r3, r3, 0xa4
@L_8011D9D4:
    cmplwi r3, 0x0
    beqlr
    rlwinm r0, r4, 0, 16, 31
    cmplwi r0, 0x1f
    ble @L_8011D9EC
    li r4, 0x1f
@L_8011D9EC:
    sth r4, 0x4(r3)
    blr

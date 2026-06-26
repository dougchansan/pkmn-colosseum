    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D99C
    li r3, 0x0
    b @L_8011D9A0
@L_8011D99C:
    addi r3, r3, 0xa4
@L_8011D9A0:
    cmplwi r3, 0x0
    beqlr
    rlwinm r0, r4, 0, 16, 31
    cmplwi r0, 0x1f
    ble @L_8011D9B8
    li r4, 0x1f
@L_8011D9B8:
    sth r4, 0x6(r3)
    blr

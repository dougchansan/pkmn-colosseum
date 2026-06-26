    nofralloc
    cmplwi r3, 0x0
    bne @L_8011DB3C
    li r3, 0x0
    b @L_8011DB40
@L_8011DB3C:
    addi r3, r3, 0x98
@L_8011DB40:
    cmplwi r3, 0x0
    beqlr
    rlwinm r0, r4, 0, 16, 31
    cmplwi r0, 0xff
    ble @L_8011DB58
    li r4, 0xff
@L_8011DB58:
    sth r4, 0x2(r3)
    blr

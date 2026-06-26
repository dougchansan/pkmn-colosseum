    nofralloc
    cmplwi r3, 0x0
    bne @L_8011DB70
    li r3, 0x0
    b @L_8011DB74
@L_8011DB70:
    addi r3, r3, 0x98
@L_8011DB74:
    cmplwi r3, 0x0
    beqlr
    rlwinm r0, r4, 0, 16, 31
    cmplwi r0, 0xff
    ble @L_8011DB8C
    li r4, 0xff
@L_8011DB8C:
    sth r4, 0x0(r3)
    blr

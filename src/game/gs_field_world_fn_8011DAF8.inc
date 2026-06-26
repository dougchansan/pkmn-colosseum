    nofralloc
    cmplwi r3, 0x0
    bne @L_8011DB08
    li r3, 0x0
    b @L_8011DB0C
@L_8011DB08:
    addi r3, r3, 0x98
@L_8011DB0C:
    cmplwi r3, 0x0
    beqlr
    rlwinm r0, r4, 0, 16, 31
    cmplwi r0, 0xff
    ble @L_8011DB24
    li r4, 0xff
@L_8011DB24:
    sth r4, 0x4(r3)
    blr

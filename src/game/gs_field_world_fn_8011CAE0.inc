    nofralloc
    cmplwi r3, 0x0
    bne @L_8011CAF0
    li r3, 0x0
    blr
@L_8011CAF0:
    rlwinm r0, r4, 0, 24, 31
    cmplwi r0, 0x3
    blt @L_8011CB04
    li r3, 0x0
    blr
@L_8011CB04:
    lbzx r0, r3, r0
    extsb r3, r0
    blr

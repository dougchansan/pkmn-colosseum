    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E454
    li r3, 0x0
    blr
@L_8011E454:
    rlwinm r0, r4, 0, 16, 31
    cmplwi r0, 0x2
    blt @L_8011E468
    li r3, 0x0
    blr
@L_8011E468:
    add r3, r3, r0
    lbz r3, 0x32(r3)
    blr

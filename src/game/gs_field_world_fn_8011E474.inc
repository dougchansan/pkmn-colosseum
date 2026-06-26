    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E484
    li r3, 0x0
    blr
@L_8011E484:
    rlwinm r0, r4, 0, 16, 31
    cmplwi r0, 0x2
    blt @L_8011E498
    li r3, 0x0
    blr
@L_8011E498:
    add r3, r3, r0
    lbz r3, 0x30(r3)
    blr

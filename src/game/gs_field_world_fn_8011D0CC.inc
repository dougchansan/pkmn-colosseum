    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D0DC
    li r4, 0x0
    b @L_8011D0FC
@L_8011D0DC:
    rlwinm r0, r4, 0, 16, 31
    cmplwi r0, 0x14
    blt @L_8011D0F0
    li r4, 0x0
    b @L_8011D0FC
@L_8011D0F0:
    clrlslwi r4, r4, 16, 2
    addi r4, r4, 0xba
    add r4, r3, r4
@L_8011D0FC:
    cmplwi r4, 0x0
    beqlr
    sth r5, 0x2(r4)
    blr

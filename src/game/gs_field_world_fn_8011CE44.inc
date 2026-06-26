    nofralloc
    cmplwi r3, 0x0
    bne @L_8011CE54
    li r3, 0x0
    blr
@L_8011CE54:
    rlwinm r0, r4, 0, 24, 31
    cmplwi r0, 0x65
    blt @L_8011CE68
    li r3, 0x0
    blr
@L_8011CE68:
    clrlslwi r0, r4, 24, 2
    lwzx r3, r3, r0
    blr

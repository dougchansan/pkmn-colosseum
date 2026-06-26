    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E058
    li r3, 0x0
    blr
@L_8011E058:
    rlwinm r0, r4, 0, 16, 31
    cmplwi r0, 0x2
    blt @L_8011E06C
    li r3, 0x0
    blr
@L_8011E06C:
    add r3, r3, r0
    lbz r3, 0x6e(r3)
    blr

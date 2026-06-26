    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E088
    li r3, 0x0
    blr
@L_8011E088:
    rlwinm r0, r4, 0, 16, 31
    cmplwi r0, 0x8
    blt @L_8011E09C
    li r3, 0x0
    blr
@L_8011E09C:
    clrlslwi r0, r4, 16, 1
    add r3, r3, r0
    lhz r3, 0x74(r3)
    blr

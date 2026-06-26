    nofralloc
    cmplwi r3, 0x0
    beqlr
    sth r4, 0x8a(r3)
    bne @L_8011DC84
    li r4, 0x0
    b @L_8011DC88
@L_8011DC84:
    addi r4, r3, 0x8c
@L_8011DC88:
    cmplwi r4, 0x0
    bne @L_8011DC98
    li r5, 0x0
    b @L_8011DC9C
@L_8011DC98:
    lhz r5, 0x0(r4)
@L_8011DC9C:
    lhz r0, 0x8a(r3)
    rlwinm r4, r5, 0, 16, 31
    cmplw r4, r0
    bgelr
    sth r5, 0x8a(r3)
    blr

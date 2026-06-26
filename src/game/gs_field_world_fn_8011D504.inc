    nofralloc
    cmplwi r3, 0x0
    beqlr
    bne @L_8011D518
    li r0, 0x0
    b @L_8011D51C
@L_8011D518:
    lhz r0, 0x0(r3)
@L_8011D51C:
    lwz r5, -30864(r13)
    rlwinm r6, r0, 0, 16, 31
    lwz r0, 0x0(r5)
    cmplw r6, r0
    blt @L_8011D538
    li r5, 0x0
    b @L_8011D544
@L_8011D538:
    mulli r0, r6, 0x11c
    lwz r5, -30860(r13)
    add r5, r5, r0
@L_8011D544:
    cmplwi r5, 0x0
    bne @L_8011D554
    li r0, 0x0
    b @L_8011D558
@L_8011D554:
    lbz r0, 0x33(r5)
@L_8011D558:
    rlwinm. r0, r0, 0, 24, 31
    bne @L_8011D564
    li r4, 0x0
@L_8011D564:
    stb r4, 0xcc(r3)
    blr

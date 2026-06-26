    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E334
    li r4, 0x0
    b @L_8011E354
@L_8011E334:
    clrlwi r0, r4, 16
    cmplwi r0, 0x14
    blt @L_8011E348
    li r4, 0x0
    b @L_8011E354
@L_8011E348:
    clrlslwi r4, r4, 16, 2
    addi r4, r4, 0xba
    add r4, r3, r4
@L_8011E354:
    cmplwi r4, 0x0
    bne @L_8011E364
    li r3, 0x0
    blr
@L_8011E364:
    lbz r3, 0x0(r4)
    blr

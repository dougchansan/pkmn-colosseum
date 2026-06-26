    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E40C
    li r4, 0x0
    b @L_8011E42C
@L_8011E40C:
    clrlwi r0, r4, 16
    cmplwi r0, 0x5
    blt @L_8011E420
    li r4, 0x0
    b @L_8011E42C
@L_8011E420:
    mulli r4, r0, 0x6
    addi r4, r4, 0x9c
    add r4, r3, r4
@L_8011E42C:
    cmplwi r4, 0x0
    bne @L_8011E43C
    li r3, 0x0
    blr
@L_8011E43C:
    lbz r3, 0x0(r4)
    blr

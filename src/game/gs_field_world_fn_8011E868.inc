    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E878
    li r3, 0x0
    blr
@L_8011E878:
    bne @L_8011E884
    li r0, 0x0
    b @L_8011E888
@L_8011E884:
    lhz r0, 0x0(r3)
@L_8011E888:
    lwz r4, lbl_80478F90(r13)
    clrlwi r5, r0, 16
    lwz r0, 0x0(r4)
    cmplw r5, r0
    blt @L_8011E8A4
    li r4, 0x0
    b @L_8011E8B0
@L_8011E8A4:
    mulli r0, r5, 0x11c
    lwz r4, lbl_80478F94(r13)
    add r4, r4, r0
@L_8011E8B0:
    cmplwi r4, 0x0
    bne @L_8011E8C0
    li r0, 0x0
    b @L_8011E8C4
@L_8011E8C0:
    lbz r0, 0x33(r4)
@L_8011E8C4:
    clrlwi. r0, r0, 24
    bne @L_8011E8D4
    li r3, 0x0
    blr
@L_8011E8D4:
    lbz r3, 0xcc(r3)
    blr

    nofralloc
    cmplwi r3, 0x0
    bne @L_8011C460
    li r3, 0x0
    blr
@L_8011C460:
    bne @L_8011C46C
    li r3, 0x0
    b @L_8011C470
@L_8011C46C:
    lhz r3, 0x1c(r3)
@L_8011C470:
    lwz r0, lbl_80478B50(r13)
    clrlwi r3, r3, 16
    cmplw r3, r0
    blt @L_8011C488
    li r3, 0x0
    b @L_8011C498
@L_8011C488:
    mulli r4, r3, 0x14
    lis r3, lbl_8035C430@ha
    addi r0, r3, lbl_8035C430@l
    add r3, r0, r4
@L_8011C498:
    cmplwi r3, 0x0
    bne @L_8011C4A8
    li r3, 0x0
    blr
@L_8011C4A8:
    lwz r3, 0x8(r3)
    blr

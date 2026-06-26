    nofralloc
    cmplwi r3, 0x0
    bne @L_8011C280
    li r3, 0x0
    blr
@L_8011C280:
    bne @L_8011C28C
    li r3, 0x0
    b @L_8011C290
@L_8011C28C:
    lhz r3, 0x1c(r3)
@L_8011C290:
    lwz r0, lbl_80478B50(r13)
    clrlwi r3, r3, 16
    cmplw r3, r0
    blt @L_8011C2A8
    li r3, 0x0
    b @L_8011C2B8
@L_8011C2A8:
    mulli r4, r3, 0x14
    lis r3, lbl_8035C430@ha
    addi r0, r3, lbl_8035C430@l
    add r3, r0, r4
@L_8011C2B8:
    cmplwi r3, 0x0
    bne @L_8011C2C8
    li r3, 0x0
    blr
@L_8011C2C8:
    lwz r3, 0x10(r3)
    blr

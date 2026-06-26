    nofralloc
    cmplwi r3, 0x0
    bne @L_8011CA70
    li r3, 0x0
    blr
@L_8011CA70:
    lhz r4, 0x10(r3)
    lwz r0, -31912(r13)
    cmplw r4, r0
    blt @L_8011CA88
    li r3, 0x0
    blr
@L_8011CA88:
    lis r3, lbl_8035F9A8@ha
    clrlslwi r0, r4, 16, 2
    addi r3, r3, lbl_8035F9A8@l
    lwzx r3, r3, r0
    blr

    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    cmplwi r3, 0x0
    bne @L_80115738
    lis r3, lbl_80272608@ha
    lis r4, lbl_8035BA48@ha
    addi r3, r3, lbl_80272608@l
    addi r4, r4, lbl_8035BA48@l
    crxor 6, 6, 6
    bl fn_800DD970
    li r3, 0x0
    b @L_80115758
@L_80115738:
    lwz r3, 0x14(r3)
    cmplwi r3, 0x0
    bne @L_8011574C
    li r3, 0x0
    b @L_80115758
@L_8011574C:
    lwz r3, 0x0(r3)
    lwz r3, 0x0(r3)
    lwz r3, 0x0(r3)
@L_80115758:
    lwz r0, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr

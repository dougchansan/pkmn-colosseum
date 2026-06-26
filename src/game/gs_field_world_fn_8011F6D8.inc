    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    stw r30, 0x8(r1)
    mr. r30, r3
    bne @L_8011F6FC
    li r3, 0x0
    b @L_8011F764
@L_8011F6FC:
    li r4, 0x0
    li r5, 0xc2
    li r6, 0x0
    bl fn_8012640C
    clrlwi. r0, r3, 24
    bne @L_8011F71C
    li r3, 0x0
    b @L_8011F764
@L_8011F71C:
    mr r3, r30
    li r4, 0x0
    li r5, 0xbf
    li r6, 0x0
    bl fn_8012640C
    clrlwi r31, r3, 24
    mr r3, r30
    bl fn_8011F77C
    clrlwi r0, r3, 24
    mr r30, r3
    cmplwi r0, 0x7
    bgt @L_8011F760
    mr r3, r31
    bl fn_8011CE18
    mr r4, r30
    bl fn_8011CC24
    b @L_8011F764
@L_8011F760:
    li r3, 0x0
@L_8011F764:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    lwz r30, 0x8(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr

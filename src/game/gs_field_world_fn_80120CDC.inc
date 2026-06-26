    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    li r4, 0x0
    li r5, 0x6e
    stw r0, 0x14(r1)
    li r6, 0x0
    stw r31, 0xc(r1)
    stw r30, 0x8(r1)
    mr r30, r3
    bl fn_8012640C
    clrlwi r4, r3, 16
    li r3, 0x0
    li r5, 0x11
    li r6, 0x0
    bl fn_8012640C
    clrlwi r31, r3, 24
    mr r3, r30
    li r4, 0x0
    li r5, 0x7a
    li r6, 0x0
    bl fn_8012640C
    clrlwi r30, r3, 24
    mr r3, r31
    bl fn_8011CE74
    cmplwi r3, 0x0
    bne @L_80120D4C
    li r3, 0x0
    b @L_80120D54
@L_80120D4C:
    mr r4, r30
    bl fn_8011CE44
@L_80120D54:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    lwz r30, 0x8(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr

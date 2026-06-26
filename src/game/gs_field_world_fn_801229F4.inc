    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    cmplwi r3, 0x0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    mr r31, r4
    bne @L_80122A18
    li r3, 0x0
    b @L_80122A5C
@L_80122A18:
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi r4, r3, 16
    li r3, 0x0
    li r5, 0x11
    li r6, 0x0
    bl fn_8012640C
    clrlwi r3, r3, 24
    bl fn_8011CE74
    cmplwi r3, 0x0
    bne @L_80122A54
    li r3, 0x0
    b @L_80122A5C
@L_80122A54:
    mr r4, r31
    bl fn_8011CE44
@L_80122A5C:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr

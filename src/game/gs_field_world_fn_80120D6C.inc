    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    cmplwi r3, 0x0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    stw r30, 0x8(r1)
    mr r30, r4
    bne @L_80120D94
    li r3, -0x1
    b @L_80120DB8
@L_80120D94:
    li r4, 0x0
    li r5, 0xbf
    li r6, 0x0
    bl fn_8012640C
    clrlwi r31, r3, 24
    mr r3, r30
    bl fn_80143B08
    mr r4, r31
    bl fn_80143ABC
@L_80120DB8:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    lwz r30, 0x8(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr

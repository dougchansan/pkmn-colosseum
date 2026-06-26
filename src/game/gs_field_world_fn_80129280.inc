    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    clrlwi r0, r4, 16
    cmplwi r0, 0x11
    stw r31, 0xc(r1)
    mr r31, r4
    blt @L_801292A8
    li r3, 0x0
    b @L_80129370
@L_801292A8:
    cmplwi r3, 0x0
    bne @L_801292C4
    bl fn_80128E24
    cmplwi r3, 0x0
    bne @L_801292C4
    li r3, 0x0
    b @L_80129370
@L_801292C4:
    clrlwi r0, r31, 16
    cmplwi r0, 0x10
    bgt @L_8012936C
    lis r4, jumptable_803634A8@ha
    slwi r0, r0, 2
    addi r4, r4, jumptable_803634A8@l
    lwzx r0, r4, r0
    mtctr r0
    bctr
    b @L_80129370
    bl fn_80128E04
    b @L_80129370
    bl fn_80128DEC
    b @L_80129370
    bl fn_80128DD4
    b @L_80129370
    bl fn_80128DB8
    b @L_80129370
    bl fn_80128D9C
    b @L_80129370
    bl fn_80128D80
    b @L_80129370
    li r3, 0x8
    b @L_80129370
    li r3, 0x20
    b @L_80129370
    li r3, 0x180
    b @L_80129370
    bl fn_80128D68
    b @L_80129370
    bl fn_80128D4C
    b @L_80129370
    bl fn_80128D30
    b @L_80129370
    bl fn_80128D14
    b @L_80129370
    bl fn_80128CF8
    b @L_80129370
    bl fn_80128CDC
    b @L_80129370
    bl fn_80128CC0
    b @L_80129370
@L_8012936C:
    li r3, 0x0
@L_80129370:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr

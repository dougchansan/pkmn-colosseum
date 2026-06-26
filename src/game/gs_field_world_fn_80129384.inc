    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    li r5, 0x0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    mr r31, r4
    li r4, 0xd
    stw r30, 0x8(r1)
    mr r30, r3
    bl fn_8012A5B0
    subf r5, r31, r3
    mr r3, r30
    li r4, 0xd
    bl fn_8012A450
    cmpwi r31, 0x0
    bgt @L_801293E4
    mr r3, r30
    li r4, 0xe
    li r5, 0x0
    bl fn_8012A5B0
    subf r5, r31, r3
    mr r3, r30
    li r4, 0xe
    bl fn_8012A450
@L_801293E4:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    lwz r30, 0x8(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr

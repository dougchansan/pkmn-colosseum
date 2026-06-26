    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    mr. r31, r3
    stw r30, 0x8(r1)
    mr r30, r4
    beq @L_80123350
    li r4, 0x0
    li r5, 0xc2
    li r6, 0x0
    bl fn_8012640C
    clrlwi. r0, r3, 24
    beq @L_80123350
    mr r3, r31
    li r4, 0x0
    li r5, 0xc3
    li r6, 0x0
    bl fn_8012640C
    clrlwi r31, r3, 16
    li r4, 0x1
    mr r3, r31
    bl fn_801EE958
    clrlwi. r0, r30, 24
    beq @L_80123350
    mr r3, r31
    li r4, 0x1
    bl fn_801EEB34
@L_80123350:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    lwz r30, 0x8(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr

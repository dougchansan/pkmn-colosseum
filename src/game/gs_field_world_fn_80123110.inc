    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    mr r31, r4
    stw r30, 0x8(r1)
    mr. r30, r3
    bne @L_80123138
    li r3, 0x0
    b @L_8012318C
@L_80123138:
    clrlwi. r0, r5, 24
    bne @L_80123174
    li r4, 0x0
    li r5, 0x82
    li r6, 0x0
    bl fn_8012640C
    clrlwi r0, r3, 16
    mr r3, r30
    mr r31, r0
    li r4, 0x0
    li r5, 0x82
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    b @L_80123188
@L_80123174:
    clrlwi r7, r31, 16
    li r4, 0x0
    li r5, 0x82
    li r6, 0x0
    bl fn_801254B4
@L_80123188:
    mr r3, r31
@L_8012318C:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    lwz r30, 0x8(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr

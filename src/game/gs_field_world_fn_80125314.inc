    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    mr r31, r4
    stw r30, 0x8(r1)
    mr. r30, r3
    beq @L_80125378
    mr r6, r31
    li r4, 0x0
    li r5, 0x7f
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    mr r6, r31
    li r4, 0x0
    li r5, 0x80
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    mr r6, r31
    li r4, 0x0
    li r5, 0x81
    li r7, 0x0
    bl fn_801254B4
@L_80125378:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    lwz r30, 0x8(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr

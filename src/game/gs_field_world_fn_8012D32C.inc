    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    lis r6, lbl_80426BD0@ha
    stw r0, 0x24(r1)
    addi r6, r6, lbl_80426BD0@l
    stw r31, 0x1c(r1)
    mr r31, r3
    lwz r6, 0x0(r6)
    lwz r5, -26224(r2)
    lwz r0, -26220(r2)
    cmpwi r6, 0x0
    stw r5, 0x8(r1)
    stw r0, 0xc(r1)
    blt @L_8012D378
    cmpwi r6, 0x2
    bge @L_8012D378
    slwi r0, r6, 2
    addi r3, r1, 0x8
    lwzx r4, r3, r0
@L_8012D378:
    li r3, 0x0
    bl fn_800F9318
    mr r4, r31
    bl fn_800E3D98
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr

    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    clrlwi r0, r3, 24
    cmpwi r0, 0x2
    stw r31, 0xc(r1)
    mr r31, r5
    beq @L_8012BB50
    bge @L_8012BB20
    cmpwi r0, 0x1
    bge @L_8012BB2C
    b @L_8012BB94
    @L_8012BB20:
    cmpwi r0, 0x4
    bge @L_8012BB94
    b @L_8012BB74
    @L_8012BB2C:
    lis r3, lbl_80426BD0@ha
    li r5, 0xd0
    addi r3, r3, lbl_80426BD0@l
    addi r3, r3, 0x270
    bl memcpy
    lis r3, lbl_80426BD0@ha
    addi r3, r3, lbl_80426BD0@l
    stw r31, 0x414(r3)
    b @L_8012BB94
    @L_8012BB50:
    lis r3, lbl_80426BD0@ha
    li r5, 0xd0
    addi r3, r3, lbl_80426BD0@l
    addi r3, r3, 0x340
    bl memcpy
    lis r3, lbl_80426BD0@ha
    addi r3, r3, lbl_80426BD0@l
    stw r31, 0x418(r3)
    b @L_8012BB94
    @L_8012BB74:
    lis r3, lbl_80426BD0@ha
    li r5, 0xd0
    addi r3, r3, lbl_80426BD0@l
    addi r3, r3, 0x1a0
    bl memcpy
    lis r3, lbl_80426BD0@ha
    addi r3, r3, lbl_80426BD0@l
    stw r31, 0x410(r3)
    @L_8012BB94:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr

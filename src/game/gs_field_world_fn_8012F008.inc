    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    mr. r31, r4
    stw r30, 0x18(r1)
    stw r29, 0x14(r1)
    blt @L_8012F030
    cmpwi r31, 0x2
    blt @L_8012F038
@L_8012F030:
    li r3, 0x0
    b @L_8012F100
@L_8012F038:
    cmpwi r3, 0x0
    blt @L_8012F048
    cmpwi r3, 0x2
    blt @L_8012F050
@L_8012F048:
    li r0, 0x0
    b @L_8012F078
@L_8012F050:
    lis r4, lbl_80426BD0@ha
    slwi r0, r3, 5
    addi r4, r4, lbl_80426BD0@l
    add r4, r4, r0
    lhz r0, 0x4(r4)
    clrlwi. r0, r0, 31
    bne @L_8012F074
    li r0, 0x0
    b @L_8012F078
@L_8012F074:
    li r0, 0x1
@L_8012F078:
    clrlwi. r0, r0, 24
    bne @L_8012F088
    li r3, 0x0
    b @L_8012F100
@L_8012F088:
    lwz r4, -26224(r2)
    cmpwi r3, 0x0
    lwz r0, -26220(r2)
    stw r4, 0x8(r1)
    stw r0, 0xc(r1)
    blt @L_8012F0B4
    cmpwi r3, 0x2
    bge @L_8012F0B4
    slwi r0, r3, 2
    addi r4, r1, 0x8
    lwzx r29, r4, r0
@L_8012F0B4:
    lis r4, lbl_80426BD0@ha
    slwi r0, r3, 5
    addi r3, r4, lbl_80426BD0@l
    add r30, r3, r0
    lwzu r0, 0xc(r30)
    cmpwi r0, 0x1
    beq @L_8012F0D4
    b @L_8012F0E0
@L_8012F0D4:
    mr r4, r29
    li r3, 0x0
    bl fn_80188AF4
@L_8012F0E0:
    cmpwi r31, 0x1
    beq @L_8012F0EC
    b @L_8012F0F8
@L_8012F0EC:
    mr r4, r29
    li r3, 0x0
    bl fn_80188F78
@L_8012F0F8:
    stw r31, 0x0(r30)
    li r3, 0x1
@L_8012F100:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr

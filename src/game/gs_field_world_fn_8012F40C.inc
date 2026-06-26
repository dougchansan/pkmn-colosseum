    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    cmpwi r3, 0x0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    blt @L_8012F42C
    cmpwi r3, 0x2
    blt @L_8012F434
@L_8012F42C:
    li r0, 0x0
    b @L_8012F45C
@L_8012F434:
    lis r5, lbl_80426BD0@ha
    slwi r0, r3, 5
    addi r5, r5, lbl_80426BD0@l
    add r5, r5, r0
    lhz r0, 0x4(r5)
    clrlwi. r0, r0, 31
    bne @L_8012F458
    li r0, 0x0
    b @L_8012F45C
@L_8012F458:
    li r0, 0x1
@L_8012F45C:
    clrlwi. r0, r0, 24
    bne @L_8012F46C
    li r3, 0x0
    b @L_8012F5FC
@L_8012F46C:
    cmpwi r3, 0x0
    lis r5, lbl_80426BD0@ha
    stwu r3, lbl_80426BD0@l(r5)
    blt @L_8012F484
    cmpwi r3, 0x2
    blt @L_8012F48C
@L_8012F484:
    li r0, 0x0
    b @L_8012F4AC
@L_8012F48C:
    slwi r0, r3, 5
    add r5, r5, r0
    lhz r0, 0x4(r5)
    clrlwi. r0, r0, 31
    bne @L_8012F4A8
    li r0, 0x0
    b @L_8012F4AC
@L_8012F4A8:
    li r0, 0x1
@L_8012F4AC:
    clrlwi. r0, r0, 24
    beq @L_8012F4CC
    lis r5, lbl_80426BD0@ha
    slwi r0, r3, 5
    addi r5, r5, lbl_80426BD0@l
    add r5, r5, r0
    lwz r0, 0xc(r5)
    b @L_8012F4D0
@L_8012F4CC:
    li r0, 0x2
@L_8012F4D0:
    cmpwi r0, 0x1
    bne @L_8012F57C
    cmpwi r3, 0x0
    blt @L_8012F4E8
    cmpwi r3, 0x2
    blt @L_8012F4F0
@L_8012F4E8:
    li r0, 0x0
    b @L_8012F518
@L_8012F4F0:
    lis r5, lbl_80426BD0@ha
    slwi r0, r3, 5
    addi r5, r5, lbl_80426BD0@l
    add r5, r5, r0
    lhz r0, 0x4(r5)
    clrlwi. r0, r0, 31
    bne @L_8012F514
    li r0, 0x0
    b @L_8012F518
@L_8012F514:
    li r0, 0x1
@L_8012F518:
    clrlwi. r0, r0, 24
    beq @L_8012F57C
    lwz r5, -26224(r2)
    cmpwi r3, 0x0
    lwz r0, -26220(r2)
    stw r5, 0x8(r1)
    stw r0, 0xc(r1)
    blt @L_8012F54C
    cmpwi r3, 0x2
    bge @L_8012F54C
    slwi r0, r3, 2
    addi r4, r1, 0x8
    lwzx r4, r4, r0
@L_8012F54C:
    lis r5, lbl_80426BD0@ha
    slwi r0, r3, 5
    addi r3, r5, lbl_80426BD0@l
    add r31, r3, r0
    lwzu r0, 0xc(r31)
    cmpwi r0, 0x1
    beq @L_8012F56C
    b @L_8012F574
@L_8012F56C:
    li r3, 0x0
    bl fn_80188AF4
@L_8012F574:
    li r0, 0x0
    stw r0, 0x0(r31)
@L_8012F57C:
    lis r3, lbl_80426BD0@ha
    lfs f1, -26216(r2)
    addi r4, r3, lbl_80426BD0@l
    lfs f2, -26060(r2)
    lwz r0, 0x0(r4)
    li r5, 0x0
    slwi r0, r0, 5
    add r3, r4, r0
    stfs f1, 0x8(r3)
    lhz r0, 0x4(r4)
    clrlwi. r0, r0, 31
    beq @L_8012F5C0
    lwz r0, 0x0(r4)
    cmpw r0, r5
    beq @L_8012F5C0
    stfs f2, 0x8(r4)
    fadds f2, f2, f2
@L_8012F5C0:
    lhz r0, 0x24(r4)
    addi r3, r4, 0x20
    li r5, 0x1
    clrlwi. r0, r0, 31
    beq @L_8012F5E4
    lwz r0, 0x0(r4)
    cmpw r0, r5
    beq @L_8012F5E4
    stfs f2, 0x8(r3)
@L_8012F5E4:
    lis r3, lbl_80426BD0@ha
    li r0, 0x0
    addi r4, r3, lbl_80426BD0@l
    li r3, 0x1
    stw r0, 0x44(r4)
    stw r0, 0x48(r4)
@L_8012F5FC:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr

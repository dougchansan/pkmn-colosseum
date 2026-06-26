    nofralloc
    stwu r1, -0x30(r1)
    mflr r0
    li r3, 0x8ae
    stw r0, 0x34(r1)
    stw r31, 0x2c(r1)
    stw r30, 0x28(r1)
    stw r29, 0x24(r1)
    li r29, 0x0
    bl fn_801906A0
    cntlzw r0, r3
    extrwi. r0, r0, 8, 19
    beq @L_801300A0
    li r3, 0x0
    li r4, 0x18
    li r5, 0x0
    bl fn_8012A5B0
    cmpwi r3, 0x0
    beq @L_801300A0
    li r29, 0x1
@L_801300A0:
    clrlwi. r0, r29, 24
    beq @L_801301B4
    lis r3, lbl_80426BD0@ha
    addi r31, r3, lbl_80426BD0@l
    lhz r3, 0x24(r31)
    clrlwi. r0, r3, 31
    bne @L_80130230
    li r29, 0x1
    ori r0, r3, 0x1
    cmpwi r29, 0x0
    sth r0, 0x24(r31)
    blt @L_8013012C
    cmpwi r29, 0x2
    bge @L_8013012C
    clrlwi. r0, r0, 31
    beq @L_8013012C
    lwz r0, 0x2c(r31)
    lwz r30, -26220(r2)
    lwz r3, -26224(r2)
    cmpwi r0, 0x1
    stw r30, 0x14(r1)
    stw r3, 0x10(r1)
    beq @L_80130100
    b @L_8013010C
@L_80130100:
    mr r4, r30
    li r3, 0x0
    bl fn_80188AF4
@L_8013010C:
    cmpwi r29, 0x1
    beq @L_80130118
    b @L_80130124
@L_80130118:
    mr r4, r30
    li r3, 0x0
    bl fn_80188F78
@L_80130124:
    li r0, 0x1
    stw r0, 0x2c(r31)
@L_8013012C:
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
    beq @L_80130170
    lwz r0, 0x0(r4)
    cmpw r0, r5
    beq @L_80130170
    stfs f2, 0x8(r4)
    fadds f2, f2, f2
@L_80130170:
    lhz r0, 0x24(r4)
    addi r3, r4, 0x20
    li r5, 0x1
    clrlwi. r0, r0, 31
    beq @L_80130194
    lwz r0, 0x0(r4)
    cmpw r0, r5
    beq @L_80130194
    stfs f2, 0x8(r3)
@L_80130194:
    lwz r4, -26220(r2)
    li r3, 0x0
    lwz r0, -26224(r2)
    li r5, 0x1
    stw r4, 0xc(r1)
    stw r0, 0x8(r1)
    bl fn_8018C1E8
    b @L_80130230
@L_801301B4:
    lis r3, lbl_80426BD0@ha
    addi r5, r3, lbl_80426BD0@l
    lwz r0, 0x0(r5)
    cmpwi r0, 0x1
    beq @L_80130230
    lhz r4, 0x24(r5)
    slwi r0, r0, 5
    add r3, r5, r0
    lfs f1, -26216(r2)
    rlwinm r0, r4, 0, 16, 30
    lfs f2, -26060(r2)
    sth r0, 0x24(r5)
    li r4, 0x0
    stfs f1, 0x8(r3)
    lhz r0, 0x4(r5)
    clrlwi. r0, r0, 31
    beq @L_8013020C
    lwz r0, 0x0(r5)
    cmpw r0, r4
    beq @L_8013020C
    stfs f2, 0x8(r5)
    fadds f2, f2, f2
@L_8013020C:
    lhz r0, 0x24(r5)
    addi r3, r5, 0x20
    li r4, 0x1
    clrlwi. r0, r0, 31
    beq @L_80130230
    lwz r0, 0x0(r5)
    cmpw r0, r4
    beq @L_80130230
    stfs f2, 0x8(r3)
@L_80130230:
    lwz r0, 0x34(r1)
    lwz r31, 0x2c(r1)
    lwz r30, 0x28(r1)
    lwz r29, 0x24(r1)
    mtlr r0
    addi r1, r1, 0x30
    blr

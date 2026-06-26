    nofralloc
    stwu r1, -0x30(r1)
    mflr r0
    li r4, 0x0
    lis r3, lbl_80426BD0@ha
    stw r0, 0x34(r1)
    clrlwi. r0, r4, 31
    stw r31, 0x2c(r1)
    addi r31, r3, lbl_80426BD0@l
    stw r30, 0x28(r1)
    stw r29, 0x24(r1)
    sth r4, 0x4(r31)
    sth r4, 0x24(r31)
    stw r4, 0x0(r31)
    stw r4, 0x188(r31)
    bne @L_8013037C
    li r29, 0x1
    ori r0, r4, 0x1
    cmpwi r29, 0x0
    sth r0, 0x4(r31)
    blt @L_801302F8
    cmpwi r29, 0x2
    bge @L_801302F8
    clrlwi. r0, r0, 31
    beq @L_801302F8
    lwz r0, 0xc(r31)
    lwz r30, -26224(r2)
    lwz r3, -26220(r2)
    cmpwi r0, 0x1
    stw r30, 0x18(r1)
    stw r3, 0x1c(r1)
    beq @L_801302CC
    b @L_801302D8
@L_801302CC:
    mr r4, r30
    li r3, 0x0
    bl fn_80188AF4
@L_801302D8:
    cmpwi r29, 0x1
    beq @L_801302E4
    b @L_801302F0
@L_801302E4:
    mr r4, r30
    li r3, 0x0
    bl fn_80188F78
@L_801302F0:
    li r0, 0x1
    stw r0, 0xc(r31)
@L_801302F8:
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
    beq @L_8013033C
    lwz r0, 0x0(r4)
    cmpw r0, r5
    beq @L_8013033C
    stfs f2, 0x8(r4)
    fadds f2, f2, f2
@L_8013033C:
    lhz r0, 0x24(r4)
    addi r3, r4, 0x20
    li r5, 0x1
    clrlwi. r0, r0, 31
    beq @L_80130360
    lwz r0, 0x0(r4)
    cmpw r0, r5
    beq @L_80130360
    stfs f2, 0x8(r3)
@L_80130360:
    lwz r4, -26224(r2)
    li r3, 0x0
    lwz r0, -26220(r2)
    li r5, 0x1
    stw r4, 0x10(r1)
    stw r0, 0x14(r1)
    bl fn_8018C1E8
@L_8013037C:
    lhz r0, 0x4(r31)
    clrlwi. r4, r0, 31
    beq @L_8013046C
    cmplwi r4, 0x0
    li r0, 0x0
    lis r3, lbl_80426BD0@ha
    stwu r0, lbl_80426BD0@l(r3)
    beq @L_801303A4
    lwz r0, 0xc(r3)
    b @L_801303A8
@L_801303A4:
    li r0, 0x2
@L_801303A8:
    cmpwi r0, 0x1
    bne @L_801303F0
    cmpwi r4, 0x0
    beq @L_801303F0
    lis r3, lbl_80426BD0@ha
    lwz r4, -26224(r2)
    addi r31, r3, lbl_80426BD0@l
    lwz r3, -26220(r2)
    lwz r0, 0xc(r31)
    stw r4, 0x8(r1)
    cmpwi r0, 0x1
    stw r3, 0xc(r1)
    beq @L_801303E0
    b @L_801303E8
@L_801303E0:
    li r3, 0x0
    bl fn_80188AF4
@L_801303E8:
    li r0, 0x0
    stw r0, 0xc(r31)
@L_801303F0:
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
    beq @L_80130434
    lwz r0, 0x0(r4)
    cmpw r0, r5
    beq @L_80130434
    stfs f2, 0x8(r4)
    fadds f2, f2, f2
@L_80130434:
    lhz r0, 0x24(r4)
    addi r3, r4, 0x20
    li r5, 0x1
    clrlwi. r0, r0, 31
    beq @L_80130458
    lwz r0, 0x0(r4)
    cmpw r0, r5
    beq @L_80130458
    stfs f2, 0x8(r3)
@L_80130458:
    lis r3, lbl_80426BD0@ha
    li r0, 0x0
    addi r3, r3, lbl_80426BD0@l
    stw r0, 0x44(r3)
    stw r0, 0x48(r3)
@L_8013046C:
    lis r3, lbl_80426BD0@ha
    lfs f0, -26216(r2)
    addi r5, r3, lbl_80426BD0@l
    li r6, 0x0
    stw r6, 0x18c(r5)
    addi r7, r5, 0x140
    stfs f0, 0x13c(r5)
    stw r6, 0x140(r5)
    stw r6, 0x148(r5)
    stw r6, 0x150(r5)
    stw r6, 0x158(r5)
    stw r6, 0x160(r5)
    stw r6, 0x168(r5)
    stw r6, 0x170(r5)
    stw r6, 0x178(r5)
    stw r6, 0x180(r5)
    lwz r0, 0x140(r5)
    cmplwi r0, 0x0
    beq @L_80130544
    addi r3, r5, 0x8
    li r6, 0x1
    lwz r0, 0x148(r5)
    cmplwi r0, 0x0
    beq @L_80130544
    lwz r0, 0x148(r3)
    li r6, 0x2
    addi r3, r3, 0x8
    cmplwi r0, 0x0
    beq @L_80130544
    lwz r0, 0x148(r3)
    li r6, 0x3
    addi r3, r3, 0x8
    cmplwi r0, 0x0
    beq @L_80130544
    lwz r0, 0x148(r3)
    li r6, 0x4
    addi r3, r3, 0x8
    cmplwi r0, 0x0
    beq @L_80130544
    lwz r0, 0x148(r3)
    li r6, 0x5
    addi r3, r3, 0x8
    cmplwi r0, 0x0
    beq @L_80130544
    lwz r0, 0x148(r3)
    li r6, 0x6
    addi r3, r3, 0x8
    cmplwi r0, 0x0
    beq @L_80130544
    lwz r0, 0x148(r3)
    li r6, 0x7
    cmplwi r0, 0x0
    beq @L_80130544
    li r6, 0x8
@L_80130544:
    cmpwi r6, 0x8
    bge @L_80130570
    lis r4, fn_8012AD50@ha
    lis r3, lbl_80426BD0@ha
    addi r4, r4, fn_8012AD50@l
    slwi r6, r6, 3
    addi r0, r3, lbl_80426BD0@l
    stwx r4, r7, r6
    add r3, r0, r6
    li r0, 0x0
    stw r0, 0x144(r3)
@L_80130570:
    lis r3, lbl_80426BD0@ha
    li r6, 0x0
    addi r3, r3, lbl_80426BD0@l
    stw r6, 0x184(r3)
    lwz r0, 0x140(r5)
    cmplwi r0, 0x0
    beq @L_80130618
    addi r3, r5, 0x8
    li r6, 0x1
    lwz r0, 0x148(r5)
    cmplwi r0, 0x0
    beq @L_80130618
    lwz r0, 0x148(r3)
    li r6, 0x2
    addi r3, r3, 0x8
    cmplwi r0, 0x0
    beq @L_80130618
    lwz r0, 0x148(r3)
    li r6, 0x3
    addi r3, r3, 0x8
    cmplwi r0, 0x0
    beq @L_80130618
    lwz r0, 0x148(r3)
    li r6, 0x4
    addi r3, r3, 0x8
    cmplwi r0, 0x0
    beq @L_80130618
    lwz r0, 0x148(r3)
    li r6, 0x5
    addi r3, r3, 0x8
    cmplwi r0, 0x0
    beq @L_80130618
    lwz r0, 0x148(r3)
    li r6, 0x6
    addi r3, r3, 0x8
    cmplwi r0, 0x0
    beq @L_80130618
    lwz r0, 0x148(r3)
    li r6, 0x7
    cmplwi r0, 0x0
    beq @L_80130618
    li r6, 0x8
@L_80130618:
    cmpwi r6, 0x8
    bge @L_80130644
    lis r4, fn_8012AC9C@ha
    lis r3, lbl_80426BD0@ha
    addi r4, r4, fn_8012AC9C@l
    slwi r5, r6, 3
    addi r0, r3, lbl_80426BD0@l
    stwx r4, r7, r5
    add r3, r0, r5
    li r0, 0x0
    stw r0, 0x144(r3)
@L_80130644:
    lwz r0, 0x34(r1)
    lwz r31, 0x2c(r1)
    lwz r30, 0x28(r1)
    lwz r29, 0x24(r1)
    mtlr r0
    addi r1, r1, 0x30
    blr

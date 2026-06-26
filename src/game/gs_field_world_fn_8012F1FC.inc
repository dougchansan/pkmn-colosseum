    nofralloc
    stwu r1, -0x30(r1)
    mflr r0
    stw r0, 0x34(r1)
    stmw r27, 0x1c(r1)
    mr. r28, r3
    blt @L_8012F21C
    cmpwi r28, 0x2
    blt @L_8012F224
@L_8012F21C:
    li r3, 0x0
    b @L_8012F3F8
@L_8012F224:
    cmpwi r28, 0x0
    blt @L_8012F234
    cmpwi r28, 0x2
    blt @L_8012F23C
@L_8012F234:
    li r0, 0x0
    b @L_8012F264
@L_8012F23C:
    lis r3, lbl_80426BD0@ha
    slwi r0, r28, 5
    addi r3, r3, lbl_80426BD0@l
    add r3, r3, r0
    lhz r0, 0x4(r3)
    clrlwi. r0, r0, 31
    bne @L_8012F260
    li r0, 0x0
    b @L_8012F264
@L_8012F260:
    li r0, 0x1
@L_8012F264:
    clrlwi. r0, r0, 24
    beq @L_8012F274
    li r3, 0x1
    b @L_8012F3F8
@L_8012F274:
    lis r3, lbl_80426BD0@ha
    li r29, 0x1
    addi r0, r3, lbl_80426BD0@l
    slwi r4, r28, 5
    add r3, r0, r4
    cmpwi r29, 0x0
    lhz r0, 0x4(r3)
    ori r0, r0, 0x1
    sthu r0, 0x4(r3)
    blt @L_8012F350
    cmpwi r29, 0x2
    bge @L_8012F350
    cmpwi r28, 0x0
    blt @L_8012F2B4
    cmpwi r28, 0x2
    blt @L_8012F2BC
@L_8012F2B4:
    li r0, 0x0
    b @L_8012F2D4
@L_8012F2BC:
    lhz r0, 0x0(r3)
    clrlwi. r0, r0, 31
    bne @L_8012F2D0
    li r0, 0x0
    b @L_8012F2D4
@L_8012F2D0:
    li r0, 0x1
@L_8012F2D4:
    clrlwi. r0, r0, 24
    beq @L_8012F350
    lwz r3, -26224(r2)
    cmpwi r28, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x10(r1)
    stw r0, 0x14(r1)
    blt @L_8012F308
    cmpwi r28, 0x2
    bge @L_8012F308
    slwi r0, r28, 2
    addi r3, r1, 0x10
    lwzx r31, r3, r0
@L_8012F308:
    lis r3, lbl_80426BD0@ha
    addi r0, r3, lbl_80426BD0@l
    add r27, r0, r4
    lwzu r0, 0xc(r27)
    cmpwi r0, 0x1
    beq @L_8012F324
    b @L_8012F330
@L_8012F324:
    mr r4, r31
    li r3, 0x0
    bl fn_80188AF4
@L_8012F330:
    cmpwi r29, 0x1
    beq @L_8012F33C
    b @L_8012F348
@L_8012F33C:
    mr r4, r31
    li r3, 0x0
    bl fn_80188F78
@L_8012F348:
    li r0, 0x1
    stw r0, 0x0(r27)
@L_8012F350:
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
    beq @L_8012F394
    lwz r0, 0x0(r4)
    cmpw r0, r5
    beq @L_8012F394
    stfs f2, 0x8(r4)
    fadds f2, f2, f2
@L_8012F394:
    lhz r0, 0x24(r4)
    addi r3, r4, 0x20
    li r5, 0x1
    clrlwi. r0, r0, 31
    beq @L_8012F3B8
    lwz r0, 0x0(r4)
    cmpw r0, r5
    beq @L_8012F3B8
    stfs f2, 0x8(r3)
@L_8012F3B8:
    lwz r3, -26224(r2)
    cmpwi r28, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x8(r1)
    stw r0, 0xc(r1)
    blt @L_8012F3E4
    cmpwi r28, 0x2
    bge @L_8012F3E4
    slwi r0, r28, 2
    addi r3, r1, 0x8
    lwzx r30, r3, r0
@L_8012F3E4:
    mr r4, r30
    li r3, 0x0
    li r5, 0x1
    bl fn_8018C1E8
    li r3, 0x1
@L_8012F3F8:
    lmw r27, 0x1c(r1)
    lwz r0, 0x34(r1)
    mtlr r0
    addi r1, r1, 0x30
    blr

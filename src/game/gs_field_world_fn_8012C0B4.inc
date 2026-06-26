    nofralloc
    stwu r1, -0x150(r1)
    mflr r0
    cmpwi r3, 0x0
    stw r0, 0x154(r1)
    stmw r24, 0x130(r1)
    lwz r4, -26224(r2)
    lwz r0, -26220(r2)
    stw r4, 0x30(r1)
    stw r0, 0x34(r1)
    blt @L_8012C0F0
    cmpwi r3, 0x2
    bge @L_8012C0F0
    slwi r0, r3, 2
    addi r3, r1, 0x30
    lwzx r26, r3, r0
@L_8012C0F0:
    lfs f1, -26160(r2)
    mr r4, r26
    lfs f2, -26156(r2)
    li r3, 0x0
    bl fn_8018CD08
    mr. r31, r3
    bne @L_8012C23C
    lis r4, lbl_80426BD0@ha
    lwz r3, -26224(r2)
    addi r4, r4, lbl_80426BD0@l
    lwz r0, -26220(r2)
    lwz r4, 0x0(r4)
    stw r3, 0x28(r1)
    cmpwi r4, 0x0
    stw r0, 0x2c(r1)
    blt @L_8012C144
    cmpwi r4, 0x2
    bge @L_8012C144
    slwi r0, r4, 2
    addi r3, r1, 0x28
    lwzx r24, r3, r0
@L_8012C144:
    mr r4, r24
    li r3, 0x0
    bl fn_8018D998
    bl fn_8018D928
    mr. r24, r3
    beq @L_8012C1F4
    bl fn_8018FCBC
    mr r25, r3
    mr r3, r24
    bl fn_8018FC50
    lwz r4, 0x0(r25)
    mr r24, r3
    lwz r0, 0x4(r25)
    lfs f0, -26152(r2)
    stw r0, 0x3c(r1)
    lfs f1, 0x3c(r1)
    stw r4, 0x38(r1)
    fadds f0, f1, f0
    lwz r0, 0x8(r25)
    stw r0, 0x40(r1)
    stfs f0, 0x3c(r1)
    lfs f1, 0x4(r3)
    bl sin
    frsp f2, f1
    lfs f1, -26148(r2)
    lfs f0, -26216(r2)
    fmuls f1, f1, f2
    stfs f0, 0x48(r1)
    stfs f1, 0x44(r1)
    lfs f1, 0x4(r24)
    bl cos
    frsp f1, f1
    lfs f0, -26148(r2)
    addi r4, r1, 0x44
    addi r3, r1, 0x38
    mr r5, r4
    fmuls f0, f0, f1
    stfs f0, 0x4c(r1)
    bl fn_800A3A78
    addi r3, r1, 0x38
    addi r4, r1, 0x44
    addi r5, r1, 0x5c
    bl fn_80111B9C
    mr r30, r3
@L_8012C1F4:
    lis r3, lbl_80426BD0@ha
    addi r4, r1, 0x5c
    addi r3, r3, lbl_80426BD0@l
    li r5, 0xd0
    addi r3, r3, 0x1a0
    bl memcpy
    lis r3, lbl_80426BD0@ha
    cmpwi r30, 0x0
    addi r3, r3, lbl_80426BD0@l
    stw r30, 0x410(r3)
    beq @L_8012C234
    li r3, 0x0
    li r4, 0x64
    bl fn_8018790C
    li r3, 0x1
    b @L_8012C52C
@L_8012C234:
    li r3, 0x0
    b @L_8012C52C
@L_8012C23C:
    li r3, 0x1
    li r4, 0x0
    li r5, 0x0
    bl fn_800F7D38
    li r3, 0x1
    li r4, 0x0
    li r5, 0x0
    bl fn_800F7C8C
    li r29, 0x0
    li r25, 0x0
    mr r24, r29
@L_8012C268:
    cmpwi r29, 0x0
    blt @L_8012C278
    cmpwi r29, 0x2
    blt @L_8012C280
@L_8012C278:
    li r0, 0x0
    b @L_8012C2A4
@L_8012C280:
    lis r3, lbl_80426BD0@ha
    addi r0, r3, lbl_80426BD0@l
    add r3, r0, r24
    lhz r0, 0x4(r3)
    clrlwi. r0, r0, 31
    bne @L_8012C2A0
    li r0, 0x0
    b @L_8012C2A4
@L_8012C2A0:
    li r0, 0x1
@L_8012C2A4:
    clrlwi. r0, r0, 24
    beq @L_8012C360
    lwz r3, -26224(r2)
    cmpwi r29, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x8(r1)
    stw r0, 0xc(r1)
    stw r3, 0x20(r1)
    stw r0, 0x24(r1)
    blt @L_8012C2DC
    cmpwi r29, 0x2
    bge @L_8012C2DC
    addi r3, r1, 0x20
    lwzx r30, r3, r25
@L_8012C2DC:
    lwz r3, 0x8(r1)
    cmpwi r29, 0x0
    lwz r0, 0xc(r1)
    stw r3, 0x10(r1)
    stw r0, 0x14(r1)
    blt @L_8012C304
    cmpwi r29, 0x2
    bge @L_8012C304
    addi r3, r1, 0x10
    lwzx r27, r3, r25
@L_8012C304:
    mr r4, r27
    li r3, 0x0
    bl fn_800F9318
    lfs f1, -26216(r2)
    mr r4, r29
    bl fn_8012C660
    lis r3, 0x8000
    mr r4, r30
    addi r5, r3, 0x8
    li r3, 0x0
    bl fn_8018C7C8
    mr r4, r30
    li r3, 0x0
    li r5, 0x100
    bl fn_8018C69C
    mr r4, r30
    li r3, 0x0
    li r5, 0x400
    bl fn_8018C69C
    mr r4, r30
    li r3, 0x0
    li r5, 0x0
    bl fn_8018CA20
@L_8012C360:
    addi r29, r29, 0x1
    addi r24, r24, 0x20
    cmpwi r29, 0x2
    addi r25, r25, 0x4
    blt @L_8012C268
    lwz r3, 0x28(r31)
    addi r5, r1, 0x50
    lwz r4, 0x2c(r31)
    bl fn_8018BA04
    lfs f1, 0x50(r1)
    mr r4, r26
    lfs f2, 0x54(r1)
    li r3, 0x0
    lfs f3, 0x58(r1)
    lfs f4, -26144(r2)
    bl fn_80187D48
    lwz r3, 0x28(r31)
    lwz r4, 0x2c(r31)
    bl fn_8018D7D0
    clrlwi. r0, r3, 24
    beq @L_8012C3C8
    lwz r3, 0x28(r31)
    li r5, 0x2
    lwz r4, 0x2c(r31)
    bl fn_80116164
    b @L_8012C448
@L_8012C3C8:
    lwz r3, 0x28(r31)
    lwz r4, 0x2c(r31)
    bl fn_8018D998
    bl fn_80183730
    lwz r3, 0x28(r31)
    lwz r4, 0x2c(r31)
    bl fn_8018397C
    mr. r24, r3
    beq @L_8012C438
    lwz r3, 0x28(r31)
    li r5, 0x1
    lwz r4, 0x2c(r31)
    bl fn_801812E8
    lwz r5, 0x28(r31)
    mr r3, r24
    lwz r6, 0x2c(r31)
    li r4, 0x4
    li r7, 0x0
    li r8, 0x0
    crxor 6, 6, 6
    bl fn_800F7434
    lwz r3, 0x28(r31)
    li r5, 0x0
    lwz r4, 0x2c(r31)
    bl fn_801812E8
    lwz r3, 0x28(r31)
    lwz r4, 0x2c(r31)
    bl fn_80189490
@L_8012C438:
    lwz r3, 0x28(r31)
    lwz r4, 0x2c(r31)
    bl fn_8018D998
    bl fn_80183688
@L_8012C448:
    li r24, 0x0
    li r26, 0x0
    li r27, 0x0
@L_8012C454:
    cmpwi r24, 0x0
    blt @L_8012C464
    cmpwi r24, 0x2
    blt @L_8012C46C
@L_8012C464:
    li r0, 0x0
    b @L_8012C490
@L_8012C46C:
    lis r3, lbl_80426BD0@ha
    addi r0, r3, lbl_80426BD0@l
    add r3, r0, r27
    lhz r0, 0x4(r3)
    clrlwi. r0, r0, 31
    bne @L_8012C48C
    li r0, 0x0
    b @L_8012C490
@L_8012C48C:
    li r0, 0x1
@L_8012C490:
    clrlwi. r0, r0, 24
    beq @L_8012C4F4
    lwz r3, -26224(r2)
    cmpwi r24, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x18(r1)
    stw r0, 0x1c(r1)
    blt @L_8012C4C0
    cmpwi r24, 0x2
    bge @L_8012C4C0
    addi r3, r1, 0x18
    lwzx r28, r3, r26
@L_8012C4C0:
    mr r4, r28
    li r3, 0x0
    li r5, 0x1
    bl fn_8018CA20
    mr r4, r28
    li r3, 0x0
    li r5, 0x700
    bl fn_8018C7C8
    lis r3, 0x8000
    mr r4, r28
    addi r5, r3, 0x8
    li r3, 0x0
    bl fn_8018C69C
@L_8012C4F4:
    addi r24, r24, 0x1
    addi r27, r27, 0x20
    cmpwi r24, 0x2
    addi r26, r26, 0x4
    blt @L_8012C454
    li r3, 0x1
    li r4, 0x0
    li r5, 0x0
    bl fn_800F7D38
    li r3, 0x1
    li r4, 0x0
    li r5, 0x0
    bl fn_800F7C8C
    li r3, 0x1
@L_8012C52C:
    lmw r24, 0x130(r1)
    lwz r0, 0x154(r1)
    mtlr r0
    addi r1, r1, 0x150
    blr

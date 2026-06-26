    nofralloc
    stwu r1, -0x40(r1)
    mflr r0
    lis r6, lbl_80426BD0@ha
    stw r0, 0x44(r1)
    addi r6, r6, lbl_80426BD0@l
    stw r31, 0x3c(r1)
    stw r30, 0x38(r1)
    stw r29, 0x34(r1)
    mr r29, r3
    lwz r6, 0x0(r6)
    lwz r5, -26224(r2)
    lwz r0, -26220(r2)
    cmpwi r6, 0x0
    stw r5, 0x8(r1)
    stw r0, 0xc(r1)
    blt @L_8012C594
    cmpwi r6, 0x2
    bge @L_8012C594
    slwi r0, r6, 2
    addi r3, r1, 0x8
    lwzx r4, r3, r0
@L_8012C594:
    li r3, 0x0
    bl fn_8018D998
    bl fn_8018D928
    mr. r31, r3
    beq @L_8012C640
    bl fn_8018FCBC
    mr r30, r3
    mr r3, r31
    bl fn_8018FC50
    lwz r4, 0x0(r30)
    mr r31, r3
    lwz r0, 0x4(r30)
    lfs f0, -26152(r2)
    stw r0, 0x20(r1)
    lfs f1, 0x20(r1)
    stw r4, 0x1c(r1)
    fadds f0, f1, f0
    lwz r0, 0x8(r30)
    stw r0, 0x24(r1)
    stfs f0, 0x20(r1)
    lfs f1, 0x4(r3)
    bl sin
    frsp f2, f1
    lfs f1, -26148(r2)
    lfs f0, -26216(r2)
    fmuls f1, f1, f2
    stfs f0, 0x14(r1)
    stfs f1, 0x10(r1)
    lfs f1, 0x4(r31)
    bl cos
    frsp f1, f1
    lfs f0, -26148(r2)
    addi r4, r1, 0x10
    addi r3, r1, 0x1c
    mr r5, r4
    fmuls f0, f0, f1
    stfs f0, 0x18(r1)
    bl fn_800A3A78
    mr r5, r29
    addi r3, r1, 0x1c
    addi r4, r1, 0x10
    bl fn_80111B9C
    mr r30, r3
@L_8012C640:
    lwz r0, 0x44(r1)
    mr r3, r30
    lwz r31, 0x3c(r1)
    lwz r30, 0x38(r1)
    lwz r29, 0x34(r1)
    mtlr r0
    addi r1, r1, 0x40
    blr

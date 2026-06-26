    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stmw r27, 0xc(r1)
    mr r28, r6
    mr r27, r5
    mr r30, r28
    b @L_80119C04
@L_80119BF0:
    cmplwi r30, 0x0
    bne @L_80119C00
    li r30, 0x0
    b @L_80119C04
@L_80119C00:
    lwz r30, 0xc(r30)
@L_80119C04:
    cmplwi r30, 0x0
    bne @L_80119C14
    li r0, 0x0
    b @L_80119C18
@L_80119C14:
    lwz r0, 0xc(r30)
@L_80119C18:
    cmplwi r0, 0x0
    bne @L_80119BF0
    mr r3, r30
    bl fn_800E3B6C
    mr. r29, r3
    beq @L_80119D7C
    bl fn_800E3BF8
    mr. r31, r3
    beq @L_80119D7C
    mr r3, r29
    bl fn_800E6DC0
    clrlwi. r0, r3, 24
    beq @L_80119C50
    lwz r30, 0x10(r30)
@L_80119C50:
    mr r3, r30
    mr r4, r28
    bl fn_800EE22C
    mr r4, r3
    addis r0, r4, 0x1
    cmplwi r0, 0xffff
    beq @L_80119D7C
    mr r3, r29
    bl fn_800EE150
    mr. r28, r3
    beq @L_80119D7C
    mr r3, r29
    bl fn_800E3CBC
    mr r5, r3
    mr r3, r31
    mr r4, r27
    bl fn_801190DC
    mr. r30, r3
    beq @L_80119D58
    mr r3, r29
    bl fn_800E3BF0
    lwz r0, 0x44(r30)
    mr r31, r3
    cmpwi r0, 0x0
    bne @L_80119D58
    cmpwi r31, 0x0
    beq @L_80119D58
    lwz r0, 0x4(r28)
    addi r3, r30, 0x50
    lfs f1, -26296(r2)
    stw r0, 0x48(r30)
    fmr f2, f1
    lhz r0, 0x2(r28)
    fmr f3, f1
    stw r0, 0x4c(r30)
    bl fn_800E01F4
    lfs f1, -26296(r2)
    addi r3, r30, 0x5c
    fmr f2, f1
    fmr f3, f1
    bl fn_800E01F4
    lfs f1, -26292(r2)
    addi r3, r30, 0x68
    fmr f2, f1
    fmr f3, f1
    bl fn_800E01F4
    lfs f1, -26296(r2)
    addi r3, r30, 0x14
    fmr f2, f1
    fmr f3, f1
    bl fn_800E01F4
    lfs f1, -26296(r2)
    addi r3, r30, 0x20
    fmr f2, f1
    fmr f3, f1
    bl fn_800E01F4
    lfs f1, -26292(r2)
    addi r3, r30, 0x2c
    fmr f2, f1
    fmr f3, f1
    bl fn_800E01F4
    stw r31, 0x44(r30)
    li r3, 0x0
    li r0, 0x1
    stb r3, 0x5(r30)
    stb r0, 0x6(r30)
@L_80119D58:
    mr r3, r28
    bl fn_800EE828
    mr r3, r29
    bl fn_800E3D08
    clrlwi. r0, r3, 24
    bne @L_80119D7C
    lwz r3, 0x10(r30)
    li r4, 0x0
    bl psSetParticleVisibility
@L_80119D7C:
    lmw r27, 0xc(r1)
    lwz r0, 0x24(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr

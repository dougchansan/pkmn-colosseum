    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stmw r27, 0xc(r1)
    mr r27, r3
    mr r28, r4
    mr r29, r5
    mr r30, r6
    mr r31, r8
    lwz r0, 0x44(r3)
    cmpwi r0, 0x0
    bne @L_801190C8
    cmpwi r29, 0x0
    beq @L_801190C8
    lwz r3, 0x4(r28)
    clrlwi r0, r7, 24
    cmplwi r0, 0x1
    stw r3, 0x48(r27)
    lhz r0, 0x2(r28)
    stw r0, 0x4c(r27)
    bne @L_80119044
    lfs f1, -26296(r2)
    addi r3, r27, 0x50
    fmr f2, f1
    fmr f3, f1
    bl fn_800E01F4
    lfs f1, -26296(r2)
    addi r3, r27, 0x5c
    fmr f2, f1
    fmr f3, f1
    bl fn_800E01F4
    lfs f1, -26292(r2)
    addi r3, r27, 0x68
    fmr f2, f1
    fmr f3, f1
    bl fn_800E01F4
    b @L_80119068
@L_80119044:
    addi r3, r27, 0x50
    addi r4, r27, 0x14
    bl fn_800E01D0
    addi r3, r27, 0x5c
    addi r4, r27, 0x20
    bl fn_800E01D0
    addi r3, r27, 0x68
    addi r4, r27, 0x2c
    bl fn_800E01D0
@L_80119068:
    lfs f1, -26296(r2)
    addi r3, r27, 0x14
    fmr f2, f1
    fmr f3, f1
    bl fn_800E01F4
    lfs f1, -26296(r2)
    addi r3, r27, 0x20
    fmr f2, f1
    fmr f3, f1
    bl fn_800E01F4
    lfs f1, -26292(r2)
    addi r3, r27, 0x2c
    fmr f2, f1
    fmr f3, f1
    bl fn_800E01F4
    clrlwi. r0, r31, 24
    beq @L_801190B8
    lwz r3, 0x10(r27)
    lwz r4, 0x8(r28)
    bl fn_80169494
@L_801190B8:
    stw r29, 0x44(r27)
    li r0, 0x1
    stb r30, 0x5(r27)
    stb r0, 0x6(r27)
@L_801190C8:
    lmw r27, 0xc(r1)
    lwz r0, 0x24(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr

    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    mr r31, r6
    stw r30, 0x18(r1)
    mr r30, r5
    stw r29, 0x14(r1)
    mr r29, r4
    stw r28, 0x10(r1)
    mr r28, r3
    lwz r0, 0x44(r3)
    cmpwi r0, 0x0
    bne @L_80118E48
    addi r3, r28, 0x2c
    bl fn_800E01D0
    lfs f0, 0x0(r29)
    lwz r3, 0x10(r28)
    stfs f0, 0x98(r3)
    lfs f0, 0x4(r29)
    lwz r3, 0x10(r28)
    stfs f0, 0x9c(r3)
    lfs f0, 0x8(r29)
    lwz r3, 0x10(r28)
    stfs f0, 0xa0(r3)
    b @L_80118E50
@L_80118E48:
    addi r3, r28, 0x68
    bl fn_800E01D0
@L_80118E50:
    clrlwi r0, r30, 24
    cmplwi r0, 0x1
    bne @L_80118E6C
    lwz r3, 0x10(r28)
    mr r4, r29
    mr r5, r31
    bl psSetGeneratorAngleRadiusScale
@L_80118E6C:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    lwz r28, 0x10(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr

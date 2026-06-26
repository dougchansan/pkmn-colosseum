    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    clrlwi r0, r4, 24
    cmplwi r0, 0x1
    stw r31, 0xc(r1)
    mr r31, r3
    bne @L_80118A98
    lwz r4, 0x10(r31)
    lhz r3, 0x18(r4)
    lbz r4, 0x15(r4)
    bl psKillFamily
@L_80118A98:
    lwz r0, 0x44(r31)
    cmpwi r0, 0x0
    beq @L_80118BBC
    beq @L_80118BBC
    lwz r3, 0x48(r31)
    li r4, 0x0
    bl fn_800EC160
    li r0, 0x0
    stw r0, 0x48(r31)
    stw r0, 0x4c(r31)
    stb r0, 0x6(r31)
    stb r0, 0x5(r31)
    lwz r3, 0x10(r31)
    bl fn_80169484
    li r0, 0x0
    stw r0, 0x44(r31)
    lwz r0, 0x44(r31)
    cmpwi r0, 0x0
    bne @L_80118B18
    addi r3, r31, 0x14
    addi r4, r31, 0x50
    bl fn_800E01D0
    lfs f0, 0x50(r31)
    lwz r3, 0x10(r31)
    stfs f0, 0x20(r3)
    lfs f0, 0x54(r31)
    lwz r3, 0x10(r31)
    stfs f0, 0x24(r3)
    lfs f0, 0x58(r31)
    lwz r3, 0x10(r31)
    stfs f0, 0x28(r3)
    b @L_80118B24
@L_80118B18:
    addi r3, r31, 0x50
    mr r4, r3
    bl fn_800E01D0
@L_80118B24:
    lwz r0, 0x44(r31)
    cmpwi r0, 0x0
    bne @L_80118B64
    addi r3, r31, 0x20
    addi r4, r31, 0x5c
    bl fn_800E01D0
    lfs f0, 0x5c(r31)
    lwz r3, 0x10(r31)
    stfs f0, 0x8c(r3)
    lfs f0, 0x60(r31)
    lwz r3, 0x10(r31)
    stfs f0, 0x90(r3)
    lfs f0, 0x64(r31)
    lwz r3, 0x10(r31)
    stfs f0, 0x94(r3)
    b @L_80118B70
@L_80118B64:
    addi r3, r31, 0x5c
    mr r4, r3
    bl fn_800E01D0
@L_80118B70:
    lwz r0, 0x44(r31)
    cmpwi r0, 0x0
    bne @L_80118BB0
    addi r3, r31, 0x2c
    addi r4, r31, 0x68
    bl fn_800E01D0
    lfs f0, 0x68(r31)
    lwz r3, 0x10(r31)
    stfs f0, 0x98(r3)
    lfs f0, 0x6c(r31)
    lwz r3, 0x10(r31)
    stfs f0, 0x9c(r3)
    lfs f0, 0x70(r31)
    lwz r3, 0x10(r31)
    stfs f0, 0xa0(r3)
    b @L_80118BBC
@L_80118BB0:
    addi r3, r31, 0x68
    mr r4, r3
    bl fn_800E01D0
@L_80118BBC:
    lwz r3, 0x10(r31)
    bl psKillGenerator
    lwz r5, 0xc(r31)
    li r0, 0x40
    li r3, 0x0
    mr r4, r5
    mtctr r0
@L_80118BD8:
    lwz r0, 0x8(r4)
    cmplw r0, r31
    bne @L_80118BF8
    slwi r0, r3, 2
    li r4, 0x0
    add r3, r5, r0
    stw r4, 0x8(r3)
    b @L_80118C04
@L_80118BF8:
    addi r4, r4, 0x4
    addi r3, r3, 0x1
    bdnz @L_80118BD8
@L_80118C04:
    li r0, 0x0
    stb r0, 0x0(r31)
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr

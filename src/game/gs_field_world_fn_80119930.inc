    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    mr r5, r3
    li r8, -0x1
    stw r0, 0x14(r1)
    li r0, 0x10
    li r9, -0x1
    li r6, 0x0
    stw r31, 0xc(r1)
    li r4, 0x0
    mtctr r0
@L_8011995C:
    lwz r7, 0x8(r5)
    lbz r0, 0x0(r7)
    cmplwi r0, 0x0
    bne @L_80119974
    stw r4, 0x8(r5)
    b @L_80119988
@L_80119974:
    lwz r0, 0x8(r7)
    cmplw r0, r8
    bge @L_80119988
    mr r8, r0
    mr r9, r6
@L_80119988:
    lwz r7, 0xc(r5)
    addi r6, r6, 0x1
    lbz r0, 0x0(r7)
    cmplwi r0, 0x0
    bne @L_801199A4
    stw r4, 0xc(r5)
    b @L_801199B8
@L_801199A4:
    lwz r0, 0x8(r7)
    cmplw r0, r8
    bge @L_801199B8
    mr r8, r0
    mr r9, r6
@L_801199B8:
    lwz r7, 0x10(r5)
    addi r6, r6, 0x1
    lbz r0, 0x0(r7)
    cmplwi r0, 0x0
    bne @L_801199D4
    stw r4, 0x10(r5)
    b @L_801199E8
@L_801199D4:
    lwz r0, 0x8(r7)
    cmplw r0, r8
    bge @L_801199E8
    mr r8, r0
    mr r9, r6
@L_801199E8:
    lwz r7, 0x14(r5)
    addi r6, r6, 0x1
    lbz r0, 0x0(r7)
    cmplwi r0, 0x0
    bne @L_80119A04
    stw r4, 0x14(r5)
    b @L_80119A18
@L_80119A04:
    lwz r0, 0x8(r7)
    cmplw r0, r8
    bge @L_80119A18
    mr r8, r0
    mr r9, r6
@L_80119A18:
    addi r5, r5, 0x10
    addi r6, r6, 0x1
    bdnz @L_8011995C
    cmpwi r9, -0x1
    beq @L_80119BBC
    slwi r0, r9, 2
    add r3, r3, r0
    lwz r31, 0x8(r3)
    lwz r4, 0x10(r31)
    lhz r3, 0x18(r4)
    lbz r4, 0x15(r4)
    bl psKillFamily
    lwz r0, 0x44(r31)
    cmpwi r0, 0x0
    beq @L_80119B6C
    beq @L_80119B6C
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
    bne @L_80119AC8
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
    b @L_80119AD4
@L_80119AC8:
    addi r3, r31, 0x50
    mr r4, r3
    bl fn_800E01D0
@L_80119AD4:
    lwz r0, 0x44(r31)
    cmpwi r0, 0x0
    bne @L_80119B14
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
    b @L_80119B20
@L_80119B14:
    addi r3, r31, 0x5c
    mr r4, r3
    bl fn_800E01D0
@L_80119B20:
    lwz r0, 0x44(r31)
    cmpwi r0, 0x0
    bne @L_80119B60
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
    b @L_80119B6C
@L_80119B60:
    addi r3, r31, 0x68
    mr r4, r3
    bl fn_800E01D0
@L_80119B6C:
    lwz r3, 0x10(r31)
    bl psKillGenerator
    lwz r5, 0xc(r31)
    li r0, 0x40
    li r3, 0x0
    mr r4, r5
    mtctr r0
@L_80119B88:
    lwz r0, 0x8(r4)
    cmplw r0, r31
    bne @L_80119BA8
    slwi r0, r3, 2
    li r4, 0x0
    add r3, r5, r0
    stw r4, 0x8(r3)
    b @L_80119BB4
@L_80119BA8:
    addi r4, r4, 0x4
    addi r3, r3, 0x1
    bdnz @L_80119B88
@L_80119BB4:
    li r0, 0x0
    stb r0, 0x0(r31)
@L_80119BBC:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr

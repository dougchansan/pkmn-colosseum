    nofralloc
    stwu r1, -0x150(r1)
    mflr r0
    li r4, 0x2
    li r5, 0x0
    stw r0, 0x154(r1)
    stw r31, 0x14c(r1)
    stw r30, 0x148(r1)
    mr r30, r3
    bl fn_8012A5B0
    mr r0, r3
    li r3, 0x0
    mr r31, r0
    li r4, 0x1
    bl gamedataGetStatus
    mr r6, r3
    addi r3, r1, 0x8
    li r4, 0xc5
    li r5, 0x1a
    bl fn_801240C4
    addi r3, r1, 0x8
    li r4, 0x0
    li r5, 0x2c
    bl fn_80123D58
    addi r3, r1, 0x8
    li r4, 0x1
    li r5, 0x122
    bl fn_80123D58
    addi r3, r1, 0x8
    li r4, 0x2
    li r5, 0x10d
    bl fn_80123D58
    addi r3, r1, 0x8
    li r4, 0x3
    li r5, 0x121
    bl fn_80123D58
    mr r7, r31
    addi r3, r1, 0x8
    li r4, 0x0
    li r5, -0x1
    li r6, 0x0
    bl fn_80124410
    mr r0, r3
    addi r3, r1, 0x8
    mr r7, r0
    li r4, 0x0
    li r5, 0x6f
    li r6, 0x0
    bl fn_801254B4
    addi r3, r1, 0x8
    li r4, 0x0
    li r5, 0x79
    li r6, 0x0
    li r7, 0x4a6f
    bl fn_801254B4
    addi r3, r1, 0x8
    li r4, 0x0
    li r5, 0x99
    li r6, 0x0
    li r7, 0xdc
    bl fn_801254B4
    addi r3, r1, 0x8
    bl fn_8012546C
    mr r3, r30
    addi r4, r1, 0x8
    li r5, 0xfe
    li r6, 0x4
    li r7, 0x0
    bl fn_80129F20
    lwz r0, 0x154(r1)
    lwz r31, 0x14c(r1)
    lwz r30, 0x148(r1)
    mtlr r0
    addi r1, r1, 0x150
    blr

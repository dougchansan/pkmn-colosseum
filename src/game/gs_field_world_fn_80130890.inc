    nofralloc
    stwu r1, -0x150(r1)
    mflr r0
    li r4, 0x5
    stw r0, 0x154(r1)
    stw r31, 0x14c(r1)
    stw r30, 0x148(r1)
    mr r30, r3
    li r3, 0x0
    bl gamedataGetStatus
    clrlwi r31, r3, 24
    li r3, 0x0
    li r4, 0x4
    bl gamedataGetStatus
    clrlwi r6, r3, 24
    mr r7, r31
    addi r3, r1, 0x8
    li r4, 0x8
    li r5, 0x3
    bl gamedataAttestCreate
    addi r3, r1, 0xc
    addi r6, r1, 0x8
    li r4, 0xfa
    li r5, 0x46
    bl fn_801240C4
    addi r3, r1, 0xc
    li r4, 0x0
    li r5, 0x99
    li r6, 0x0
    li r7, 0x46
    bl fn_801254B4
    li r3, 0x12ad
    bl fn_800FA280
    mr r9, r3
    addi r3, r1, 0xc
    li r4, 0xff
    li r5, 0x46
    li r6, 0x4
    li r7, 0x0
    li r8, 0x2740
    bl fn_80123EF0
    addi r3, r1, 0xc
    li r4, -0x1
    li r5, -0x1
    li r6, 0x0
    li r7, 0x2740
    bl fn_80124410
    mr r0, r3
    addi r3, r1, 0xc
    mr r7, r0
    li r4, 0x0
    li r5, 0x6f
    li r6, 0x0
    bl fn_801254B4
    addi r3, r1, 0xc
    bl fn_8012546C
    addi r4, r1, 0xc
    li r3, 0x0
    bl memoDataSet
    mr r3, r30
    addi r4, r1, 0xc
    li r5, 0x1
    bl fn_80129E20
    lwz r0, 0x154(r1)
    lwz r31, 0x14c(r1)
    lwz r30, 0x148(r1)
    mtlr r0
    addi r1, r1, 0x150
    blr

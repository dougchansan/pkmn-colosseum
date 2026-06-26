    nofralloc
    stwu r1, -0x30(r1)
    mflr r0
    stw r0, 0x34(r1)
    stw r31, 0x2c(r1)
    stw r30, 0x28(r1)
    mr r30, r3
    stw r29, 0x24(r1)
    mr r29, r4
    bl fn_80129094
    cmplwi r30, 0x0
    mr r3, r30
    bne @L_80128E7C
    bl fn_80128E24
    cmplwi r3, 0x0
    bne @L_80128E7C
    li r31, 0x0
    b @L_80128E84
@L_80128E7C:
    bl fn_80128E04
    mr r31, r3
@L_80128E84:
    bl fn_800057A0
    cmpwi r3, 0x1
    beq @L_80128EC8
    bge @L_80128EA0
    cmpwi r3, 0x0
    bge @L_80128EAC
    b @L_80128EFC
@L_80128EA0:
    cmpwi r3, 0x3
    bge @L_80128EFC
    b @L_80128EE4
@L_80128EAC:
    mr r3, r31
    li r4, 0xb
    li r5, 0x3
    li r6, 0x1
    li r7, 0x1
    bl gamedataCreate
    b @L_80128EFC
@L_80128EC8:
    mr r3, r31
    li r4, 0xb
    li r5, 0x3
    li r6, 0x2
    li r7, 0x2
    bl gamedataCreate
    b @L_80128EFC
@L_80128EE4:
    mr r3, r31
    li r4, 0xb
    li r5, 0x3
    li r6, 0x3
    li r7, 0x8
    bl gamedataCreate
@L_80128EFC:
    cmplwi r30, 0x0
    mr r3, r30
    bne @L_80128F1C
    bl fn_80128E24
    cmplwi r3, 0x0
    bne @L_80128F1C
    li r3, 0x0
    b @L_80128F20
@L_80128F1C:
    bl fn_80128DEC
@L_80128F20:
    cmplwi r29, 0x0
    mr r30, r3
    bne @L_80128F3C
    addi r3, r1, 0x8
    la r4, -26232(r2)
    bl fn_800F9D04
    addi r29, r1, 0x8
@L_80128F3C:
    mr r3, r30
    mr r4, r29
    li r5, 0x0
    bl fn_8012A1A4
    mr r3, r31
    li r4, 0x5
    li r5, 0x2
    bl fn_80135030
    mr r3, r31
    li r4, 0x7
    li r5, 0x1
    bl fn_80135030
    mr r3, r31
    li r4, 0x8
    li r5, 0x1
    bl fn_80135030
    mr r3, r30
    li r4, 0x0
    bl fn_80130BB0
    mr r3, r30
    li r4, 0x0
    bl fn_80130A88
    mr r3, r30
    li r4, 0x0
    bl fn_8012AC08
    mr r0, r3
    li r3, 0x0
    mr r4, r0
    bl memoDataSet
    mr r3, r30
    li r4, 0x1
    bl fn_8012AC08
    mr r4, r3
    li r3, 0x0
    bl memoDataSet
    mr r3, r30
    li r4, 0xc
    li r5, 0x2710
    bl fn_8012A450
    mr r3, r30
    li r4, 0x16
    li r5, 0x2
    li r6, -0x1
    bl fn_80129A78
    mr r3, r30
    li r4, 0xd
    li r5, 0x5
    li r6, -0x1
    bl fn_80129A78
    mr r3, r30
    li r4, 0xe
    li r5, 0x2
    li r6, -0x1
    bl fn_80129A78
    mr r3, r30
    li r4, 0xf
    li r5, 0x2
    li r6, -0x1
    bl fn_80129A78
    mr r3, r30
    li r4, 0x10
    li r5, 0x2
    li r6, -0x1
    bl fn_80129A78
    mr r3, r30
    li r4, 0x12
    li r5, 0x2
    li r6, -0x1
    bl fn_80129A78
    mr r3, r30
    li r4, 0x11
    li r5, 0x2
    li r6, -0x1
    bl fn_80129A78
    mr r3, r30
    li r4, 0x17
    li r5, 0x2
    li r6, -0x1
    bl fn_80129A78
    lwz r0, 0x34(r1)
    lwz r31, 0x2c(r1)
    lwz r30, 0x28(r1)
    lwz r29, 0x24(r1)
    mtlr r0
    addi r1, r1, 0x30
    blr

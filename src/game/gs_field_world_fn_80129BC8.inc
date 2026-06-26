    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    clrlwi r0, r4, 24
    cmpwi r0, 0x4
    li r4, 0x0
    stw r31, 0x1c(r1)
    mr r31, r8
    stw r30, 0x18(r1)
    mr r30, r7
    li r7, 0x0
    stw r29, 0x14(r1)
    mr r29, r6
    li r6, 0x0
    stw r28, 0x10(r1)
    mr r28, r5
    li r5, 0x0
    beq @L_80129CA8
    bge @L_80129C2C
    cmpwi r0, 0x2
    beq @L_80129C60
    bge @L_80129C84
    cmpwi r0, 0x1
    bge @L_80129C3C
    b @L_80129D10
@L_80129C2C:
    cmpwi r0, 0x6
    beq @L_80129CF0
    bge @L_80129D10
    b @L_80129CCC
@L_80129C3C:
    li r4, 0x6
    li r5, 0x0
    bl fn_8012A5B0
    mr r4, r3
    li r5, 0x10
    li r6, 0x63
    li r7, 0x0
    li r3, 0x1
    b @L_80129D10
@L_80129C60:
    li r4, 0x4
    li r5, 0x0
    bl fn_8012A5B0
    mr r4, r3
    li r5, 0x14
    li r6, 0x63
    li r7, 0x0
    li r3, 0x1
    b @L_80129D10
@L_80129C84:
    li r4, 0x8
    li r5, 0x0
    bl fn_8012A5B0
    mr r4, r3
    li r5, 0x2e
    li r6, 0x3e7
    li r7, 0x1
    li r3, 0x1
    b @L_80129D10
@L_80129CA8:
    li r4, 0x7
    li r5, 0x0
    bl fn_8012A5B0
    mr r4, r3
    li r5, 0x40
    li r6, 0x63
    li r7, 0x1
    li r3, 0x0
    b @L_80129D10
@L_80129CCC:
    li r4, 0x5
    li r5, 0x0
    bl fn_8012A5B0
    mr r4, r3
    li r5, 0x2b
    li r6, 0x63
    li r7, 0x0
    li r3, 0x0
    b @L_80129D10
@L_80129CF0:
    li r4, 0x9
    li r5, 0x0
    bl fn_8012A5B0
    mr r4, r3
    li r5, 0x3
    li r6, 0x63
    li r7, 0x1
    li r3, 0x0
@L_80129D10:
    cmplwi r28, 0x0
    beq @L_80129D1C
    sth r5, 0x0(r28)
@L_80129D1C:
    cmplwi r29, 0x0
    beq @L_80129D28
    sth r6, 0x0(r29)
@L_80129D28:
    cmplwi r30, 0x0
    beq @L_80129D34
    stb r7, 0x0(r30)
@L_80129D34:
    cmplwi r31, 0x0
    beq @L_80129D40
    stb r3, 0x0(r31)
@L_80129D40:
    lwz r0, 0x24(r1)
    mr r3, r4
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    lwz r28, 0x10(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr

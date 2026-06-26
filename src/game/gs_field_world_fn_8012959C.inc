    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    li r7, 0x0
    stw r0, 0x24(r1)
    addic. r0, r1, 0xc
    stw r31, 0x1c(r1)
    mr r31, r6
    stw r30, 0x18(r1)
    mr r30, r5
    stw r29, 0x14(r1)
    mr r29, r4
    sth r7, 0xc(r1)
    sth r7, 0xa(r1)
    beq @L_801295DC
    li r0, 0xa
    sth r0, 0xc(r1)
@L_801295DC:
    addic. r0, r1, 0xa
    beq @L_801295EC
    li r0, 0x1
    sth r0, 0xa(r1)
@L_801295EC:
    addic. r0, r1, 0x8
    beq @L_801295FC
    li r0, 0x0
    stb r0, 0x8(r1)
@L_801295FC:
    li r4, 0xa
    li r5, 0x0
    bl fn_8012A5B0
    cmplwi r3, 0x0
    bne @L_80129618
    li r3, -0x1
    b @L_80129634
@L_80129618:
    lhz r4, 0xc(r1)
    mr r5, r29
    lhz r8, 0xa(r1)
    mr r6, r30
    lbz r9, 0x8(r1)
    mr r7, r31
    bl fn_80140ACC
@L_80129634:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr

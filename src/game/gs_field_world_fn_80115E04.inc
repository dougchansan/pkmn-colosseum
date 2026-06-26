    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    lwz r6, lbl_80478EB8(r13)
    lwz r0, 0x0(r6)
    cmplw r3, r0
    blt @L_80115E40
    lis r3, lbl_80272708@ha
    lis r4, lbl_8035BB70@ha
    addi r3, r3, lbl_80272708@l
    addi r4, r4, lbl_8035BB70@l
    crxor 6, 6, 6
    bl fn_800DD970
    li r3, -0x1
    b @L_80115E5C
    @L_80115E40:
    mulli r6, r3, 0x1c
    lwz r7, lbl_80478EBC(r13)
    clrlwi r0, r4, 16
    li r3, 0x0
    add r4, r7, r6
    stw r0, 0xc(r4)
    stb r5, 0x1(r4)
    @L_80115E5C:
    lwz r0, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr

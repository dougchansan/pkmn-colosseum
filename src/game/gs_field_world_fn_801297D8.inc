    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    cmplwi r4, 0x0
    stw r0, 0x14(r1)
    beq @L_801297F4
    li r0, 0xa
    sth r0, 0x0(r4)
@L_801297F4:
    cmplwi r5, 0x0
    beq @L_80129804
    li r0, 0x1
    sth r0, 0x0(r5)
@L_80129804:
    cmplwi r6, 0x0
    beq @L_80129814
    li r0, 0x0
    stb r0, 0x0(r6)
@L_80129814:
    cmplwi r7, 0x0
    beq @L_80129824
    li r0, 0x1
    stb r0, 0x0(r7)
@L_80129824:
    li r4, 0xa
    li r5, 0x0
    bl fn_8012A5B0
    lwz r0, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr

    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    bl fn_801155CC
    lfs f0, -26320(r2)
    li r4, 0x0
    li r0, 0x1
    cmplwi r3, 0x0
    stw r4, lbl_8047AD68(r13)
    stw r4, lbl_8047AD6C(r13)
    stb r4, lbl_8047AD70(r13)
    stb r0, lbl_8047AD71(r13)
    stfs f0, lbl_8047AD74(r13)
    stfs f0, lbl_8047AD78(r13)
    stfs f0, lbl_8047AD7C(r13)
    beq @L_801171B8
    lwz r4, 0x0(r3)
    lwz r0, 0x0(r4)
    stw r0, lbl_8047AD68(r13)
    lwz r0, 0x4(r3)
    stw r0, lbl_8047AD6C(r13)
@L_801171B8:
    lwz r0, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr

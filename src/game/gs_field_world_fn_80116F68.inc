    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    cmplwi r3, 0x0
    bne @L_80116F84
    li r3, 0x0
    b @L_80116FD0
@L_80116F84:
    cmplwi r4, 0x0
    bne @L_80116F94
    li r3, 0x0
    b @L_80116FD0
@L_80116F94:
    lha r5, 0x4(r3)
    lis r0, 0x4330
    lfs f1, -26336(r2)
    mr r3, r4
    xoris r4, r5, 0x8000
    stw r0, 0x8(r1)
    lfd f2, -26328(r2)
    fmr f3, f1
    stw r4, 0xc(r1)
    lfs f4, -26332(r2)
    lfd f0, 0x8(r1)
    fsubs f0, f0, f2
    fmuls f2, f4, f0
    bl fn_800E01F4
    li r3, 0x1
@L_80116FD0:
    lwz r0, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr

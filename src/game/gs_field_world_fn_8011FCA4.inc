    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    mr r31, r4
    stw r30, 0x18(r1)
    mr. r30, r3
    beq @L_8011FDB0
    li r4, 0x0
    li r5, 0xc3
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    cmplwi r30, 0x0
    beq @L_8011FCF8
    mr r3, r30
    li r4, 0x0
    li r5, 0xc5
    li r6, 0x0
    li r7, -0x64
    bl fn_801254B4
@L_8011FCF8:
    mr r3, r30
    li r4, 0x0
    li r5, 0xc6
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xc7
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xc8
    li r6, 0x0
    bl fn_8012640C
    li r4, 0x1
    bl fn_8011B950
    mr r3, r30
    clrlwi r7, r31, 16
    li r4, 0x0
    li r5, 0xc3
    li r6, 0x0
    bl fn_801254B4
    mr r3, r31
    bl fn_801EEEB8
    clrlwi r3, r3, 16
    lis r0, 0x4330
    stw r3, 0xc(r1)
    cmplwi r30, 0x0
    lfd f1, -26256(r2)
    stw r0, 0x8(r1)
    lfd f0, 0x8(r1)
    fsubs f1, f0, f1
    beq @L_8011FDB0
    lfs f0, -26284(r2)
    mr r3, r30
    li r4, 0x0
    li r5, 0xc5
    fmuls f0, f0, f1
    li r6, 0x0
    fctiwz f0, f0
    stfd f0, 0x8(r1)
    lwz r7, 0xc(r1)
    bl fn_801254B4
@L_8011FDB0:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr

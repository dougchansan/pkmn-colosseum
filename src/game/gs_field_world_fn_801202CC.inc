    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    li r5, 0x7a
    li r6, 0x0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    mr r31, r4
    li r4, 0x0
    stw r30, 0x18(r1)
    mr r30, r3
    stw r29, 0x14(r1)
    stw r28, 0x10(r1)
    bl fn_8012640C
    cmplwi r30, 0x0
    clrlwi r28, r3, 24
    bne @L_80120314
    li r29, 0x0
    b @L_80120360
@L_80120314:
    mr r3, r30
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi r4, r3, 16
    li r3, 0x0
    li r5, 0x11
    li r6, 0x0
    bl fn_8012640C
    clrlwi r3, r3, 24
    bl fn_8011CE74
    cmplwi r3, 0x0
    bne @L_80120354
    li r3, 0x0
    b @L_8012035C
@L_80120354:
    mr r4, r28
    bl fn_8011CE44
@L_8012035C:
    mr r29, r3
@L_80120360:
    mr r3, r30
    li r4, 0x0
    li r5, 0xc2
    li r6, 0x0
    bl fn_8012640C
    clrlwi. r0, r3, 24
    bne @L_80120400
    cmplwi r30, 0x0
    bne @L_8012038C
    li r3, 0x0
    b @L_801203D8
@L_8012038C:
    mr r3, r30
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi r4, r3, 16
    li r3, 0x0
    li r5, 0x11
    li r6, 0x0
    bl fn_8012640C
    clrlwi r3, r3, 24
    bl fn_8011CE74
    cmplwi r3, 0x0
    bne @L_801203CC
    li r3, 0x0
    b @L_801203D8
@L_801203CC:
    addi r0, r28, 0x1
    clrlwi r4, r0, 24
    bl fn_8011CE44
@L_801203D8:
    subf r0, r29, r3
    mr r3, r30
    stw r0, 0x1c(r31)
    li r4, 0x0
    li r5, 0x79
    li r6, 0x0
    bl fn_8012640C
    subf r0, r29, r3
    stw r0, 0x20(r31)
    b @L_80120488
@L_80120400:
    mr r3, r30
    li r4, 0x0
    li r5, 0xc3
    li r6, 0x0
    bl fn_8012640C
    clrlwi r3, r3, 16
    bl fn_801EEEB8
    clrlwi r3, r3, 16
    lis r0, 0x4330
    stw r3, 0xc(r1)
    cmplwi r30, 0x0
    lfd f1, -26256(r2)
    stw r0, 0x8(r1)
    lfd f0, 0x8(r1)
    fsubs f0, f0, f1
    stfs f0, 0x1c(r31)
    bne @L_8012044C
    lfs f0, -26288(r2)
    b @L_80120484
@L_8012044C:
    mr r3, r30
    li r4, 0x0
    li r5, 0xc5
    li r6, 0x0
    bl fn_8012640C
    xoris r3, r3, 0x8000
    lis r0, 0x4330
    stw r3, 0xc(r1)
    lfd f2, -26264(r2)
    stw r0, 0x8(r1)
    lfs f0, -26284(r2)
    lfd f1, 0x8(r1)
    fsubs f1, f1, f2
    fdivs f0, f1, f0
@L_80120484:
    stfs f0, 0x20(r31)
@L_80120488:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    lwz r28, 0x10(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr

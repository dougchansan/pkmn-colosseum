    nofralloc
    stwu r1, -0x40(r1)
    mflr r0
    stw r0, 0x44(r1)
    stfd f31, 0x30(r1)
    psq_st f31, 0x38(r1), 0, 0
    stmw r27, 0x1c(r1)
    mr r27, r4
    mr r30, r5
    mr r29, r3
    li r4, 0x0
    li r5, 0xc2
    li r6, 0x0
    bl fn_8012640C
    clrlwi. r0, r3, 24
    beq @L_8011FBB0
    mr r3, r29
    li r4, 0x0
    li r5, 0xbf
    li r6, 0x0
    bl fn_8012640C
    clrlwi r3, r3, 24
    bl fn_8011CE18
    mr. r28, r3
    beq @L_8011FBB0
    mr r3, r30
    bl fn_8011CAB8
    bl fn_8011CA9C
    extsb r0, r3
    lis r3, 0x4330
    xoris r4, r0, 0x8000
    clrlwi r0, r30, 16
    stw r4, 0xc(r1)
    cmplwi r0, 0x4
    lfd f1, -26264(r2)
    stw r3, 0x8(r1)
    lfd f0, 0x8(r1)
    fsubs f31, f0, f1
    bne @L_8011F9EC
    mr r3, r27
    bl fn_801440A0
    mr. r27, r3
    beq @L_8011FBB0
    bl fn_80144014
    clrlwi r0, r3, 24
    cmplwi r0, 0x6
    bne @L_8011FBB0
    mr r3, r27
    bl fn_80143E14
    lis r0, 0x4330
    stw r3, 0xc(r1)
    lfd f1, -26256(r2)
    stw r0, 0x8(r1)
    lfd f0, 0x8(r1)
    fsubs f0, f0, f1
    fmuls f31, f31, f0
@L_8011F9EC:
    clrlwi r0, r30, 16
    cmplwi r0, 0x5
    bne @L_8011FA48
    cmplwi r29, 0x0
    bne @L_8011FA08
    lfs f1, -26288(r2)
    b @L_8011FA40
@L_8011FA08:
    mr r3, r29
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
    fdivs f1, f1, f0
@L_8011FA40:
    lfs f0, -26248(r2)
    fmuls f31, f0, f1
@L_8011FA48:
    clrlwi. r0, r30, 16
    bne @L_8011FA60
    mr r3, r28
    bl fn_8011CD6C
    mr r31, r3
    b @L_8011FABC
@L_8011FA60:
    cmplwi r0, 0x1
    bne @L_8011FA78
    mr r3, r28
    bl fn_8011CD50
    mr r31, r3
    b @L_8011FABC
@L_8011FA78:
    cmplwi r0, 0x2
    bne @L_8011FA90
    mr r3, r28
    bl fn_8011CD34
    mr r31, r3
    b @L_8011FABC
@L_8011FA90:
    cmplwi r0, 0x3
    bne @L_8011FAA8
    mr r3, r28
    bl fn_8011CD18
    mr r31, r3
    b @L_8011FABC
@L_8011FAA8:
    cmplwi r0, 0x4
    bne @L_8011FABC
    mr r3, r28
    bl fn_8011CCFC
    mr r31, r3
@L_8011FABC:
    clrlwi r3, r31, 24
    bl fn_8011CBC8
    mr. r28, r3
    beq @L_8011FBB0
    bl fn_8011CBB0
    mr r30, r3
    mr r3, r28
    bl fn_8011CB98
    clrlwi. r4, r3, 24
    beq @L_8011FBB0
    lis r0, 0x4330
    clrlwi r3, r30, 24
    stw r3, 0xc(r1)
    lfd f2, -26256(r2)
    stw r0, 0x8(r1)
    lfd f0, 0x8(r1)
    stw r4, 0x14(r1)
    fsubs f1, f0, f2
    stw r0, 0x10(r1)
    lfd f0, 0x10(r1)
    fmuls f31, f31, f1
    fsubs f0, f0, f2
    fdivs f31, f31, f0
    b @L_8011FB20
    b @L_8011FBB0
@L_8011FB20:
    cmplwi r29, 0x0
    beq @L_8011FBB0
    bne @L_8011FB34
    lfs f0, -26288(r2)
    b @L_8011FB6C
@L_8011FB34:
    mr r3, r29
    li r4, 0x0
    li r5, 0xc5
    li r6, 0x0
    bl fn_8012640C
    xoris r3, r3, 0x8000
    lis r0, 0x4330
    stw r3, 0x14(r1)
    lfd f2, -26264(r2)
    stw r0, 0x10(r1)
    lfs f0, -26284(r2)
    lfd f1, 0x10(r1)
    fsubs f1, f1, f2
    fdivs f0, f1, f0
@L_8011FB6C:
    fadds f1, f0, f31
    lfs f0, -26288(r2)
    fcmpo cr0, f1, f0
    bge @L_8011FB80
    fmr f1, f0
@L_8011FB80:
    cmplwi r29, 0x0
    beq @L_8011FBB0
    lfs f0, -26284(r2)
    mr r3, r29
    li r4, 0x0
    li r5, 0xc5
    fmuls f0, f0, f1
    li r6, 0x0
    fctiwz f0, f0
    stfd f0, 0x10(r1)
    lwz r7, 0x14(r1)
    bl fn_801254B4
@L_8011FBB0:
    psq_l f31, 0x38(r1), 0, 0
    lfd f31, 0x30(r1)
    lmw r27, 0x1c(r1)
    lwz r0, 0x44(r1)
    mtlr r0
    addi r1, r1, 0x40
    blr

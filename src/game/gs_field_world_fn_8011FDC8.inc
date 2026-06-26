    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    li r5, 0x6e
    li r6, 0x0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    mr r31, r3
    stw r30, 0x18(r1)
    mr r30, r4
    li r4, 0x0
    stw r29, 0x14(r1)
    stw r28, 0x10(r1)
    bl fn_8012640C
    mr r3, r31
    li r4, 0x0
    li r5, 0x77
    li r6, 0x0
    bl fn_8012640C
    mr r4, r3
    mr r3, r30
    bl fn_800F9E70
    mr r3, r31
    li r4, 0x0
    li r5, 0x7a
    li r6, 0x0
    bl fn_8012640C
    stb r3, 0x17(r30)
    mr r3, r31
    li r4, 0x0
    li r5, 0x87
    li r6, 0x0
    bl fn_8012640C
    sth r3, 0x18(r30)
    mr r3, r31
    li r4, 0x0
    li r5, 0x83
    li r6, 0x0
    bl fn_8012640C
    sth r3, 0x1a(r30)
    mr r3, r31
    li r4, 0x0
    li r5, 0x7a
    li r6, 0x0
    bl fn_8012640C
    cmplwi r31, 0x0
    clrlwi r29, r3, 24
    bne @L_8011FE8C
    li r28, 0x0
    b @L_8011FED8
@L_8011FE8C:
    mr r3, r31
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
    bne @L_8011FECC
    li r3, 0x0
    b @L_8011FED4
@L_8011FECC:
    mr r4, r29
    bl fn_8011CE44
@L_8011FED4:
    mr r28, r3
@L_8011FED8:
    mr r3, r31
    li r4, 0x0
    li r5, 0xc2
    li r6, 0x0
    bl fn_8012640C
    clrlwi. r0, r3, 24
    bne @L_8011FF7C
    cmplwi r31, 0x0
    addi r0, r29, 0x1
    clrlwi r29, r0, 24
    bne @L_8011FF0C
    li r3, 0x0
    b @L_8011FF54
@L_8011FF0C:
    mr r3, r31
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
    bne @L_8011FF4C
    li r3, 0x0
    b @L_8011FF54
@L_8011FF4C:
    mr r4, r29
    bl fn_8011CE44
@L_8011FF54:
    subf r0, r28, r3
    mr r3, r31
    stw r0, 0x1c(r30)
    li r4, 0x0
    li r5, 0x79
    li r6, 0x0
    bl fn_8012640C
    subf r0, r28, r3
    stw r0, 0x20(r30)
    b @L_80120004
@L_8011FF7C:
    mr r3, r31
    li r4, 0x0
    li r5, 0xc3
    li r6, 0x0
    bl fn_8012640C
    clrlwi r3, r3, 16
    bl fn_801EEEB8
    clrlwi r3, r3, 16
    lis r0, 0x4330
    stw r3, 0xc(r1)
    cmplwi r31, 0x0
    lfd f1, -26256(r2)
    stw r0, 0x8(r1)
    lfd f0, 0x8(r1)
    fsubs f0, f0, f1
    stfs f0, 0x1c(r30)
    bne @L_8011FFC8
    lfs f0, -26288(r2)
    b @L_80120000
@L_8011FFC8:
    mr r3, r31
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
@L_80120000:
    stfs f0, 0x20(r30)
@L_80120004:
    cmplwi r31, 0x0
    bne @L_80120014
    li r0, 0x0
    b @L_801201F8
@L_80120014:
    li r3, 0x3
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80120044
    li r3, 0x3
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80120044
    li r3, 0x0
    b @L_80120050
@L_80120044:
    mr r3, r31
    li r4, 0x3
    bl fn_8011B67C
@L_80120050:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80120064
    li r0, 0x3a
    b @L_801201F8
@L_80120064:
    li r3, 0x4
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80120094
    li r3, 0x4
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80120094
    li r3, 0x0
    b @L_801200A0
@L_80120094:
    mr r3, r31
    li r4, 0x4
    bl fn_8011B67C
@L_801200A0:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_801200B4
    li r0, 0x3a
    b @L_801201F8
@L_801200B4:
    li r3, 0x5
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_801200E4
    li r3, 0x5
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_801200E4
    li r3, 0x0
    b @L_801200F0
@L_801200E4:
    mr r3, r31
    li r4, 0x5
    bl fn_8011B67C
@L_801200F0:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80120104
    li r0, 0x3b
    b @L_801201F8
@L_80120104:
    li r3, 0x6
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80120134
    li r3, 0x6
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80120134
    li r3, 0x0
    b @L_80120140
@L_80120134:
    mr r3, r31
    li r4, 0x6
    bl fn_8011B67C
@L_80120140:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80120154
    li r0, 0x3c
    b @L_801201F8
@L_80120154:
    li r3, 0x7
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80120184
    li r3, 0x7
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80120184
    li r3, 0x0
    b @L_80120190
@L_80120184:
    mr r3, r31
    li r4, 0x7
    bl fn_8011B67C
@L_80120190:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_801201A4
    li r0, 0x3d
    b @L_801201F8
@L_801201A4:
    li r3, 0x8
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_801201D4
    li r3, 0x8
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_801201D4
    li r3, 0x0
    b @L_801201E0
@L_801201D4:
    mr r3, r31
    li r4, 0x8
    bl fn_8011B67C
@L_801201E0:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_801201F4
    li r0, 0x3e
    b @L_801201F8
@L_801201F4:
    li r0, 0x0
@L_801201F8:
    sth r0, 0x24(r30)
    mr r3, r31
    bl fn_8010BBB8
    sth r3, 0x26(r30)
    mr r3, r31
    bl menuSubGetPokemonSexForFightDisp
    stb r3, 0x28(r30)
    mr r3, r31
    li r4, 0x0
    li r5, 0xc2
    li r6, 0x0
    bl fn_8012640C
    clrlwi. r0, r3, 24
    bne @L_8012023C
    li r0, 0x0
    stb r0, 0x29(r30)
    b @L_80120294
@L_8012023C:
    li r3, 0x3e
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_8012026C
    li r3, 0x3e
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_8012026C
    li r3, 0x0
    b @L_80120278
@L_8012026C:
    mr r3, r31
    li r4, 0x3e
    bl fn_8011B67C
@L_80120278:
    clrlwi. r0, r3, 24
    bne @L_8012028C
    li r0, 0x1
    stb r0, 0x29(r30)
    b @L_80120294
@L_8012028C:
    li r0, 0x2
    stb r0, 0x29(r30)
@L_80120294:
    mr r3, r31
    li r4, 0x0
    li r5, 0x82
    li r6, 0x0
    bl fn_8012640C
    sth r3, 0x2a(r30)
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    lwz r28, 0x10(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr

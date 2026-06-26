    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    mr. r31, r3
    bne @L_80121218
    li r3, 0x0
    b @L_801213FC
@L_80121218:
    li r3, 0x3
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80121248
    li r3, 0x3
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80121248
    li r3, 0x0
    b @L_80121254
@L_80121248:
    mr r3, r31
    li r4, 0x3
    bl fn_8011B67C
@L_80121254:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80121268
    li r3, 0x3
    b @L_801213FC
@L_80121268:
    li r3, 0x4
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80121298
    li r3, 0x4
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80121298
    li r3, 0x0
    b @L_801212A4
@L_80121298:
    mr r3, r31
    li r4, 0x4
    bl fn_8011B67C
@L_801212A4:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_801212B8
    li r3, 0x4
    b @L_801213FC
@L_801212B8:
    li r3, 0x5
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_801212E8
    li r3, 0x5
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_801212E8
    li r3, 0x0
    b @L_801212F4
@L_801212E8:
    mr r3, r31
    li r4, 0x5
    bl fn_8011B67C
@L_801212F4:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80121308
    li r3, 0x5
    b @L_801213FC
@L_80121308:
    li r3, 0x6
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80121338
    li r3, 0x6
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80121338
    li r3, 0x0
    b @L_80121344
@L_80121338:
    mr r3, r31
    li r4, 0x6
    bl fn_8011B67C
@L_80121344:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80121358
    li r3, 0x6
    b @L_801213FC
@L_80121358:
    li r3, 0x7
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80121388
    li r3, 0x7
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80121388
    li r3, 0x0
    b @L_80121394
@L_80121388:
    mr r3, r31
    li r4, 0x7
    bl fn_8011B67C
@L_80121394:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_801213A8
    li r3, 0x7
    b @L_801213FC
@L_801213A8:
    li r3, 0x8
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_801213D8
    li r3, 0x8
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_801213D8
    li r3, 0x0
    b @L_801213E4
@L_801213D8:
    mr r3, r31
    li r4, 0x8
    bl fn_8011B67C
@L_801213E4:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_801213F8
    li r3, 0x8
    b @L_801213FC
@L_801213F8:
    li r3, 0x0
@L_801213FC:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr

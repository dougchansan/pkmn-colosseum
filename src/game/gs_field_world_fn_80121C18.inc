    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    li r4, 0x0
    li r5, 0x6f
    stw r0, 0x24(r1)
    li r6, 0x0
    stw r31, 0x1c(r1)
    mr r31, r3
    stw r30, 0x18(r1)
    stw r29, 0x14(r1)
    stw r28, 0x10(r1)
    bl fn_8012640C
    mr r0, r3
    mr r3, r31
    mr r30, r0
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi. r28, r3, 16
    bne @L_80121C74
    li r0, 0x0
    b @L_80121CB4
@L_80121C74:
    mr r4, r28
    li r3, 0x0
    li r5, 0x1
    li r6, 0x0
    bl fn_8012640C
    cmplwi r3, 0x0
    bne @L_80121C98
    li r0, 0x0
    b @L_80121CB4
@L_80121C98:
    lwz r3, lbl_80478F90(r13)
    lwz r0, 0x0(r3)
    cmplw r28, r0
    blt @L_80121CB0
    li r0, 0x0
    b @L_80121CB4
@L_80121CB0:
    li r0, 0x1
@L_80121CB4:
    clrlwi. r0, r0, 24
    bne @L_80121CC4
    li r3, 0x0
    b @L_80122020
@L_80121CC4:
    mr r4, r28
    li r3, 0x0
    li r5, 0x66
    li r6, 0x0
    bl fn_8012640C
    cmplwi r31, 0x0
    mr r28, r3
    bne @L_80121CEC
    li r5, 0x0
    b @L_80121D48
@L_80121CEC:
    mr r3, r31
    li r4, 0x0
    li r5, 0x75
    li r6, 0x0
    bl fn_8012640C
    mr r29, r3
    mr r3, r31
    li r4, 0x0
    li r5, 0x6f
    li r6, 0x0
    bl fn_8012640C
    srwi r4, r29, 16
    clrlwi r0, r29, 16
    srwi r5, r3, 16
    clrlwi r6, r3, 16
    xor r0, r4, r0
    li r3, 0x8
    xor r0, r5, r0
    xor r0, r6, r0
    xor r0, r3, r0
    cntlzw r0, r0
    slw r0, r3, r0
    srwi r5, r0, 31
@L_80121D48:
    mr r4, r30
    clrlwi r3, r28, 16
    bl fn_801DE190
    mr r0, r3
    li r3, 0x8
    mr r30, r0
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80121D8C
    li r3, 0x8
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80121D8C
    li r3, 0x0
    b @L_80121D98
@L_80121D8C:
    mr r3, r31
    li r4, 0x8
    bl fn_8011B67C
@L_80121D98:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80121DB0
    mr r3, r30
    li r4, 0x1
    bl fn_801DA3CC
@L_80121DB0:
    li r3, 0x7
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80121DE0
    li r3, 0x7
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80121DE0
    li r3, 0x0
    b @L_80121DEC
@L_80121DE0:
    mr r3, r31
    li r4, 0x7
    bl fn_8011B67C
@L_80121DEC:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80121E04
    mr r3, r30
    li r4, 0x2
    bl fn_801DA3CC
@L_80121E04:
    cmplwi r31, 0x0
    bne @L_80121E14
    li r0, 0x0
    b @L_80121FF8
@L_80121E14:
    li r3, 0x3
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80121E44
    li r3, 0x3
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80121E44
    li r3, 0x0
    b @L_80121E50
@L_80121E44:
    mr r3, r31
    li r4, 0x3
    bl fn_8011B67C
@L_80121E50:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80121E64
    li r0, 0x0
    b @L_80121FF8
@L_80121E64:
    li r3, 0x4
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80121E94
    li r3, 0x4
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80121E94
    li r3, 0x0
    b @L_80121EA0
@L_80121E94:
    mr r3, r31
    li r4, 0x4
    bl fn_8011B67C
@L_80121EA0:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80121EB4
    li r0, 0x0
    b @L_80121FF8
@L_80121EB4:
    li r3, 0x5
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80121EE4
    li r3, 0x5
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80121EE4
    li r3, 0x0
    b @L_80121EF0
@L_80121EE4:
    mr r3, r31
    li r4, 0x5
    bl fn_8011B67C
@L_80121EF0:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80121F04
    li r0, 0x0
    b @L_80121FF8
@L_80121F04:
    li r3, 0x6
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80121F34
    li r3, 0x6
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80121F34
    li r3, 0x0
    b @L_80121F40
@L_80121F34:
    mr r3, r31
    li r4, 0x6
    bl fn_8011B67C
@L_80121F40:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80121F54
    li r0, 0x0
    b @L_80121FF8
@L_80121F54:
    li r3, 0x7
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80121F84
    li r3, 0x7
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80121F84
    li r3, 0x0
    b @L_80121F90
@L_80121F84:
    mr r3, r31
    li r4, 0x7
    bl fn_8011B67C
@L_80121F90:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80121FA4
    li r0, 0x0
    b @L_80121FF8
@L_80121FA4:
    li r3, 0x8
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80121FD4
    li r3, 0x8
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80121FD4
    li r3, 0x0
    b @L_80121FE0
@L_80121FD4:
    mr r3, r31
    li r4, 0x8
    bl fn_8011B67C
@L_80121FE0:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80121FF4
    li r0, 0x0
    b @L_80121FF8
@L_80121FF4:
    li r0, 0x1
@L_80121FF8:
    clrlwi r0, r0, 24
    cmplwi r0, 0x1
    bne @L_8012201C
    mr r3, r30
    li r4, 0x1
    bl fn_801DA36C
    mr r3, r30
    li r4, 0x2
    bl fn_801DA36C
@L_8012201C:
    mr r3, r30
@L_80122020:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    lwz r28, 0x10(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr

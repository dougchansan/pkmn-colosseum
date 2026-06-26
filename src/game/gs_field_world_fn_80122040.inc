    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    mr r31, r4
    stw r30, 0x8(r1)
    mr r30, r3
    li r3, 0x8
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_8012208C
    li r3, 0x8
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_8012208C
    li r3, 0x0
    b @L_80122098
@L_8012208C:
    mr r3, r30
    li r4, 0x8
    bl fn_8011B67C
@L_80122098:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_801220B0
    mr r3, r31
    li r4, 0x1
    bl fn_801DA3CC
@L_801220B0:
    li r3, 0x7
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_801220E0
    li r3, 0x7
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_801220E0
    li r3, 0x0
    b @L_801220EC
@L_801220E0:
    mr r3, r30
    li r4, 0x7
    bl fn_8011B67C
@L_801220EC:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80122104
    mr r3, r31
    li r4, 0x2
    bl fn_801DA3CC
@L_80122104:
    cmplwi r30, 0x0
    bne @L_80122114
    li r0, 0x0
    b @L_801222F8
@L_80122114:
    li r3, 0x3
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80122144
    li r3, 0x3
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80122144
    li r3, 0x0
    b @L_80122150
@L_80122144:
    mr r3, r30
    li r4, 0x3
    bl fn_8011B67C
@L_80122150:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80122164
    li r0, 0x0
    b @L_801222F8
@L_80122164:
    li r3, 0x4
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80122194
    li r3, 0x4
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80122194
    li r3, 0x0
    b @L_801221A0
@L_80122194:
    mr r3, r30
    li r4, 0x4
    bl fn_8011B67C
@L_801221A0:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_801221B4
    li r0, 0x0
    b @L_801222F8
@L_801221B4:
    li r3, 0x5
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_801221E4
    li r3, 0x5
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_801221E4
    li r3, 0x0
    b @L_801221F0
@L_801221E4:
    mr r3, r30
    li r4, 0x5
    bl fn_8011B67C
@L_801221F0:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80122204
    li r0, 0x0
    b @L_801222F8
@L_80122204:
    li r3, 0x6
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80122234
    li r3, 0x6
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80122234
    li r3, 0x0
    b @L_80122240
@L_80122234:
    mr r3, r30
    li r4, 0x6
    bl fn_8011B67C
@L_80122240:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80122254
    li r0, 0x0
    b @L_801222F8
@L_80122254:
    li r3, 0x7
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80122284
    li r3, 0x7
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80122284
    li r3, 0x0
    b @L_80122290
@L_80122284:
    mr r3, r30
    li r4, 0x7
    bl fn_8011B67C
@L_80122290:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_801222A4
    li r0, 0x0
    b @L_801222F8
@L_801222A4:
    li r3, 0x8
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_801222D4
    li r3, 0x8
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_801222D4
    li r3, 0x0
    b @L_801222E0
@L_801222D4:
    mr r3, r30
    li r4, 0x8
    bl fn_8011B67C
@L_801222E0:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_801222F4
    li r0, 0x0
    b @L_801222F8
@L_801222F4:
    li r0, 0x1
@L_801222F8:
    clrlwi r0, r0, 24
    cmplwi r0, 0x1
    bne @L_8012231C
    mr r3, r31
    li r4, 0x1
    bl fn_801DA36C
    mr r3, r31
    li r4, 0x2
    bl fn_801DA36C
@L_8012231C:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    lwz r30, 0x8(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr

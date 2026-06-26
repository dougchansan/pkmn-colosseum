    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    clrlwi. r0, r5, 16
    stw r31, 0xc(r1)
    mr r31, r6
    stw r30, 0x8(r1)
    mr r30, r5
    beq @L_8011BEE0
    cmplwi r0, 0x35
    blt @L_8011BEE8
@L_8011BEE0:
    li r3, 0x0
    b @L_8011C1B8
@L_8011BEE8:
    cmplwi r0, 0x25
    bge @L_8011BF08
    mr r3, r4
    bl fn_8011CA34
    cmplwi r3, 0x0
    bne @L_8011BF18
    li r3, 0x0
    b @L_8011C1B8
@L_8011BF08:
    cmplwi r3, 0x0
    bne @L_8011BF18
    li r3, 0x0
    b @L_8011C1B8
@L_8011BF18:
    clrlwi r0, r30, 16
    cmplwi r0, 0x34
    bgt @L_8011C1B4
    lis r4, jumptable_8035C35C@ha
    slwi r0, r0, 2
    addi r4, r4, jumptable_8035C35C@l
    lwzx r0, r4, r0
    mtctr r0
    bctr
    bl fn_8011CA1C
    b @L_8011C1B8
    bl fn_8011CA04
    clrlwi r3, r3, 24
    b @L_8011C1B8
    bl fn_8011C9EC
    clrlwi r3, r3, 24
    b @L_8011C1B8
    bl fn_8011C9D0
    extsb r3, r3
    b @L_8011C1B8
    bl fn_8011C9B8
    clrlwi r3, r3, 24
    b @L_8011C1B8
    bl fn_8011C9A0
    clrlwi r3, r3, 24
    b @L_8011C1B8
    bl fn_8011C988
    extsh r3, r3
    b @L_8011C1B8
    bl fn_8011C970
    clrlwi r3, r3, 16
    b @L_8011C1B8
    bl fn_8011C958
    clrlwi r3, r3, 16
    b @L_8011C1B8
    bl fn_8011C940
    b @L_8011C1B8
    bl fn_8011C928
    b @L_8011C1B8
    bl fn_8011C910
    clrlwi r3, r3, 24
    b @L_8011C1B8
    bl fn_8011C8F8
    clrlwi r3, r3, 24
    b @L_8011C1B8
    bl fn_8011C8E0
    clrlwi r3, r3, 24
    b @L_8011C1B8
    bl fn_8011C8C8
    clrlwi r3, r3, 24
    b @L_8011C1B8
    bl fn_8011C8B0
    clrlwi r3, r3, 24
    b @L_8011C1B8
    bl fn_8011C898
    clrlwi r3, r3, 24
    b @L_8011C1B8
    bl fn_8011C880
    clrlwi r3, r3, 24
    b @L_8011C1B8
    bl fn_8011C868
    clrlwi r3, r3, 24
    b @L_8011C1B8
    bl fn_8011C850
    clrlwi r3, r3, 24
    b @L_8011C1B8
    bl fn_8011C838
    clrlwi r3, r3, 24
    b @L_8011C1B8
    bl fn_8011C820
    clrlwi r3, r3, 24
    b @L_8011C1B8
    bl fn_8011C808
    clrlwi r3, r3, 24
    b @L_8011C1B8
    bl fn_8011C7F0
    clrlwi r3, r3, 24
    b @L_8011C1B8
    bl fn_8011C5C8
    clrlwi r3, r3, 24
    b @L_8011C1B8
    mr r4, r31
    bl fn_8011C588
    clrlwi r3, r3, 24
    b @L_8011C1B8
    bl fn_8011C570
    clrlwi r3, r3, 24
    b @L_8011C1B8
    bl fn_8011C510
    b @L_8011C1B8
    bl fn_8011C4B0
    b @L_8011C1B8
    bl fn_8011C450
    b @L_8011C1B8
    bl fn_8011C2D0
    b @L_8011C1B8
    bl fn_8011C270
    b @L_8011C1B8
    bl fn_8011C7D8
    b @L_8011C1B8
    bl fn_8011C7C0
    b @L_8011C1B8
    bl fn_8011C7A8
    clrlwi r3, r3, 24
    b @L_8011C1B8
    bl fn_8011C790
    clrlwi r3, r3, 24
    b @L_8011C1B8
    bl fn_8020A2A0
    extsb r3, r3
    b @L_8011C1B8
    bl fn_8020A288
    clrlwi r3, r3, 16
    b @L_8011C1B8
    bl fn_8020A270
    clrlwi r3, r3, 16
    b @L_8011C1B8
    bl fn_8020A258
    clrlwi r3, r3, 16
    b @L_8011C1B8
    mr r4, r31
    bl fn_8020A224
    b @L_8011C1B8
    bl fn_8020A20C
    clrlwi r3, r3, 24
    b @L_8011C1B8
    bl fn_8020A1F4
    clrlwi r3, r3, 24
    b @L_8011C1B8
    bl fn_8020A1DC
    b @L_8011C1B8
    bl fn_8020A1C4
    b @L_8011C1B8
    bl fn_8020A1AC
    clrlwi r3, r3, 16
    b @L_8011C1B8
    bl fn_8020A194
    clrlwi r3, r3, 16
    b @L_8011C1B8
    bl fn_8020A17C
    clrlwi r3, r3, 24
    b @L_8011C1B8
    bl fn_8020A164
    clrlwi r3, r3, 24
    b @L_8011C1B8
    li r4, 0x0
    li r5, 0x27
    li r6, 0x0
    bl fn_8011BEB4
    mr r0, r3
    li r3, 0x0
    clrlwi r4, r0, 16
    li r5, 0x3
    li r6, 0x0
    bl fn_8011BEB4
    b @L_8011C1B8
    li r4, 0x0
    li r5, 0x28
    li r6, 0x0
    bl fn_8011BEB4
    mr r0, r3
    li r3, 0x0
    clrlwi r4, r0, 16
    li r5, 0x3
    li r6, 0x0
    bl fn_8011BEB4
    b @L_8011C1B8
@L_8011C1B4:
    li r3, 0x0
@L_8011C1B8:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    lwz r30, 0x8(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr

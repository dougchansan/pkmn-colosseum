    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    mr r0, r3
    mulli r3, r0, 0x108
    stw r31, 0xc(r1)
    mr r31, r4
    stw r0, lbl_8047ADA0(r13)
    bl fn_800E3534
    clrlwi. r0, r3, 16
    sth r3, lbl_8047AD98(r13)
    beq @L_8011991C
    mr r3, r0
    bl fn_800E27B0
    li r5, 0x0
    stw r3, lbl_8047AD9C(r13)
    mr r4, r5
    li r6, 0x0
    b @L_80119880
    @L_80119870:
    lwz r3, lbl_8047AD9C(r13)
    addi r6, r6, 0x1
    stbx r4, r3, r5
    addi r5, r5, 0x108
    @L_80119880:
    lwz r0, lbl_8047ADA0(r13)
    cmplw r6, r0
    blt @L_80119870
    mulli r3, r31, 0x74
    stw r31, lbl_8047ADAC(r13)
    bl fn_800E3534
    clrlwi. r0, r3, 16
    sth r3, lbl_8047ADA4(r13)
    beq @L_8011991C
    mr r3, r0
    bl fn_800E27B0
    li r6, 0x0
    stw r3, lbl_8047ADA8(r13)
    mr r5, r6
    mr r4, r6
    b @L_801198D0
    @L_801198C0:
    lwz r3, lbl_8047ADA8(r13)
    addi r6, r6, 0x1
    stbx r4, r3, r5
    addi r5, r5, 0x74
    @L_801198D0:
    lwz r0, lbl_8047ADAC(r13)
    cmplw r6, r0
    blt @L_801198C0
    li r3, 0x0
    bl fn_8016A01C
    li r3, 0x0
    bl fn_80175DF0
    li r3, 0x0
    li r4, 0x74
    bl fn_8016AAF4
    lis r3, fn_80119BD0@ha
    addi r3, r3, fn_80119BD0@l
    bl fn_8019733C
    lis r3, fn_80119BD0@ha
    addi r3, r3, fn_80119BD0@l
    bl fn_8019D618
    lis r3, fn_8016972C@ha
    addi r3, r3, fn_8016972C@l
    bl fn_8019D610
    @L_8011991C:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr

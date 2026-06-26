    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    clrlwi. r0, r5, 16
    stw r31, 0x1c(r1)
    mr r31, r7
    stw r30, 0x18(r1)
    mr r30, r6
    stw r29, 0x14(r1)
    mr r29, r5
    beq @L_8011BE98
    cmplwi r0, 0x35
    blt @L_8011BC10
    b @L_8011BE98
@L_8011BC10:
    cmplwi r0, 0x25
    bge @L_8011BC2C
    mr r3, r4
    bl fn_8011CA34
    cmplwi r3, 0x0
    bne @L_8011BC34
    b @L_8011BE98
@L_8011BC2C:
    cmplwi r3, 0x0
    beq @L_8011BE98
@L_8011BC34:
    clrlwi r0, r29, 16
    cmplwi r0, 0x32
    bgt @L_8011BE98
    lis r4, jumptable_8035C290@ha
    slwi r0, r0, 2
    addi r4, r4, jumptable_8035C290@l
    lwzx r0, r4, r0
    mtctr r0
    bctr
    mr r4, r31
    bl fn_8011C780
    b @L_8011BE98
    clrlwi r4, r31, 24
    bl fn_8011C770
    b @L_8011BE98
    clrlwi r4, r31, 24
    bl fn_8011C760
    b @L_8011BE98
    extsb r4, r31
    bl fn_8011C750
    b @L_8011BE98
    clrlwi r4, r31, 24
    bl fn_8011C740
    b @L_8011BE98
    clrlwi r4, r31, 24
    bl fn_8011C730
    b @L_8011BE98
    extsh r4, r31
    bl fn_8011C720
    b @L_8011BE98
    clrlwi r4, r31, 16
    bl fn_8011C710
    b @L_8011BE98
    clrlwi r4, r31, 16
    bl fn_8011C700
    b @L_8011BE98
    clrlwi r4, r31, 16
    bl fn_8011C6F0
    b @L_8011BE98
    clrlwi r4, r31, 16
    bl fn_8011C6F0
    b @L_8011BE98
    clrlwi r4, r31, 24
    bl fn_8011C6E0
    b @L_8011BE98
    clrlwi r4, r31, 24
    bl fn_8011C6D0
    b @L_8011BE98
    clrlwi r4, r31, 24
    bl fn_8011C6C0
    b @L_8011BE98
    clrlwi r4, r31, 24
    bl fn_8011C6B0
    b @L_8011BE98
    clrlwi r4, r31, 24
    bl fn_8011C6A0
    b @L_8011BE98
    clrlwi r4, r31, 24
    bl fn_8011C690
    b @L_8011BE98
    clrlwi r4, r31, 24
    bl fn_8011C680
    b @L_8011BE98
    clrlwi r4, r31, 24
    bl fn_8011C670
    b @L_8011BE98
    clrlwi r4, r31, 24
    bl fn_8011C660
    b @L_8011BE98
    clrlwi r4, r31, 24
    bl fn_8011C650
    b @L_8011BE98
    clrlwi r4, r31, 24
    bl fn_8011C640
    b @L_8011BE98
    clrlwi r4, r31, 24
    bl fn_8011C630
    b @L_8011BE98
    clrlwi r4, r31, 24
    bl fn_8011C620
    b @L_8011BE98
    clrlwi r4, r31, 24
    bl fn_8011C5B8
    b @L_8011BE98
    mr r4, r30
    clrlwi r5, r31, 24
    bl fn_8011C430
    b @L_8011BE98
    clrlwi r4, r31, 24
    bl fn_8011C420
    b @L_8011BE98
    mr r4, r31
    bl fn_8011C3D0
    b @L_8011BE98
    mr r4, r31
    bl fn_8011C380
    b @L_8011BE98
    mr r4, r31
    bl fn_8011C330
    b @L_8011BE98
    mr r4, r31
    bl fn_8011C220
    b @L_8011BE98
    mr r4, r31
    bl fn_8011C1D0
    b @L_8011BE98
    mr r4, r31
    bl fn_8011C610
    b @L_8011BE98
    mr r4, r31
    bl fn_8011C600
    b @L_8011BE98
    clrlwi r4, r31, 24
    bl fn_8011C5F0
    b @L_8011BE98
    clrlwi r4, r31, 24
    bl fn_8011C5E0
    b @L_8011BE98
    extsb r4, r31
    bl fn_8020A154
    b @L_8011BE98
    clrlwi r4, r31, 16
    bl fn_8020A144
    b @L_8011BE98
    clrlwi r4, r31, 16
    bl fn_8020A134
    b @L_8011BE98
    clrlwi r4, r31, 16
    bl fn_8020A124
    b @L_8011BE98
    clrlwi r4, r31, 24
    bl fn_8020A114
    b @L_8011BE98
    clrlwi r4, r31, 24
    bl fn_8020A104
    b @L_8011BE98
    mr r4, r31
    bl fn_8020A0F4
    b @L_8011BE98
    mr r4, r31
    bl fn_8020A0E4
    b @L_8011BE98
    clrlwi r4, r31, 16
    bl fn_8020A0D4
    b @L_8011BE98
    clrlwi r4, r31, 16
    bl fn_8020A0C4
    b @L_8011BE98
    clrlwi r4, r31, 24
    bl fn_8020A0B4
    b @L_8011BE98
    clrlwi r4, r31, 24
    bl fn_8020A0A4
@L_8011BE98:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr

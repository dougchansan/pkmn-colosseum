    nofralloc
    stwu r1, -0x40(r1)
    mflr r0
    stw r0, 0x44(r1)
    stfd f31, 0x30(r1)
    psq_st f31, 0x38(r1), 0, 0
    stmw r27, 0x1c(r1)
    mr r28, r3
    lfs f0, -26368(r2)
    li r30, -0x1
    lwz r3, lbl_80478EC8(r13)
    li r29, 0x0
    stfs f0, 0x8(r1)
    lwz r0, 0x0(r3)
    cmplw r4, r0
    blt @L_8011699C
    li r3, -0x1
    b @L_80116D14
@L_8011699C:
    clrlwi r31, r5, 24
    lwz r3, lbl_80478ECC(r13)
    mulli r0, r4, 0x18
    cmpwi r31, 0x2
    add r27, r3, r0
    beq @L_80116A3C
    bge @L_801169C8
    cmpwi r31, 0x0
    beq @L_801169D4
    bge @L_80116A1C
    b @L_80116A7C
@L_801169C8:
    cmpwi r31, 0x4
    bge @L_80116A7C
    b @L_80116A60
@L_801169D4:
    lhz r3, 0xe(r27)
    cmplwi r3, 0x0
    beq @L_801169F8
    bl fn_801902E0
    clrlwi r0, r3, 24
    cmplwi r0, 0x0
    beq @L_801169F8
    li r3, 0x0
    b @L_80116D14
@L_801169F8:
    lhz r3, 0x10(r27)
    cmplwi r3, 0x0
    beq @L_80116A1C
    bl fn_801902E0
    clrlwi r0, r3, 24
    cmplwi r0, 0x0
    bne @L_80116A1C
    li r3, 0x1
    b @L_80116D14
@L_80116A1C:
    lhz r3, 0xe(r27)
    cmplwi r3, 0x0
    beq @L_80116A2C
    bl fn_80190528
@L_80116A2C:
    lha r3, 0xa(r27)
    li r4, 0x0
    bl fn_801CAA08
    b @L_80116A7C
@L_80116A3C:
    lhz r3, 0xe(r27)
    cmplwi r3, 0x0
    beq @L_80116A60
    bl fn_801902E0
    clrlwi r0, r3, 24
    cmplwi r0, 0x0
    bne @L_80116A60
    li r3, 0x0
    b @L_80116D14
@L_80116A60:
    lhz r3, 0xe(r27)
    cmplwi r3, 0x0
    beq @L_80116A70
    bl fn_801903B0
@L_80116A70:
    lha r3, 0xa(r27)
    li r4, 0x1
    bl fn_801CAA08
@L_80116A7C:
    lwz r4, 0x14(r27)
    cmplwi r4, 0x0
    bne @L_80116A90
    li r3, -0x1
    b @L_80116D14
@L_80116A90:
    mr r3, r28
    bl fn_80113F6C
    mr r28, r3
    cmplwi r28, 0x0
    bne @L_80116AAC
    li r3, -0x1
    b @L_80116D14
@L_80116AAC:
    cmpwi r31, 0x2
    beq @L_80116B68
    bge @L_80116AC8
    cmpwi r31, 0x0
    beq @L_80116AD4
    bge @L_80116B1C
    b @L_80116BF8
@L_80116AC8:
    cmpwi r31, 0x4
    bge @L_80116BF8
    b @L_80116BB0
@L_80116AD4:
    lbz r0, 0x7(r27)
    lbz r30, 0x0(r27)
    cmpwi r0, 0x2
    extsb r30, r30
    beq @L_80116B0C
    bge @L_80116AF8
    cmpwi r0, 0x1
    bge @L_80116B04
    b @L_80116BF8
@L_80116AF8:
    cmpwi r0, 0x4
    bge @L_80116BF8
    b @L_80116B14
@L_80116B04:
    li r29, 0x44
    b @L_80116BF8
@L_80116B0C:
    li r29, 0x44
    b @L_80116BF8
@L_80116B14:
    li r29, 0x4be
    b @L_80116BF8
@L_80116B1C:
    lbz r30, 0x2(r27)
    extsb r30, r30
    cmpwi r30, 0x0
    bge @L_80116BF8
    lbz r30, 0x0(r27)
    extsb r30, r30
    cmpwi r30, 0x0
    blt @L_80116BF8
    mr r4, r30
    bl fn_800ECCA8
    mr r3, r28
    addi r4, r1, 0x8
    li r5, 0x0
    bl fn_800EC4D0
    lfs f1, 0x8(r1)
    lfs f0, -26356(r2)
    fsubs f0, f1, f0
    stfs f0, 0x8(r1)
    b @L_80116BF8
@L_80116B68:
    lbz r0, 0x7(r27)
    lbz r30, 0x1(r27)
    cmpwi r0, 0x2
    extsb r30, r30
    beq @L_80116BA0
    bge @L_80116B8C
    cmpwi r0, 0x1
    bge @L_80116B98
    b @L_80116BF8
@L_80116B8C:
    cmpwi r0, 0x4
    bge @L_80116BF8
    b @L_80116BA8
@L_80116B98:
    li r29, 0x44
    b @L_80116BF8
@L_80116BA0:
    li r29, 0x44
    b @L_80116BF8
@L_80116BA8:
    li r29, 0x4be
    b @L_80116BF8
@L_80116BB0:
    lbz r30, 0x3(r27)
    extsb r30, r30
    cmpwi r30, 0x0
    bge @L_80116BF8
    lbz r30, 0x1(r27)
    extsb r30, r30
    cmpwi r30, 0x0
    blt @L_80116BF8
    mr r4, r30
    bl fn_800ECCA8
    mr r3, r28
    addi r4, r1, 0x8
    li r5, 0x0
    bl fn_800EC4D0
    lfs f1, 0x8(r1)
    lfs f0, -26356(r2)
    fsubs f0, f1, f0
    stfs f0, 0x8(r1)
@L_80116BF8:
    extsh r0, r30
    cmpwi r0, 0x0
    bge @L_80116C0C
    li r3, -0x1
    b @L_80116D14
@L_80116C0C:
    mr r3, r28
    bl fn_800EC1BC
    clrlwi r0, r3, 24
    cmplwi r0, 0x0
    bne @L_80116C28
    li r3, -0x1
    b @L_80116D14
@L_80116C28:
    cmplwi r28, 0x0
    lfs f31, 0x8(r1)
    beq @L_80116C6C
    mr r3, r28
    extsh r4, r30
    bl fn_800ECCA8
    fmr f1, f31
    mr r3, r28
    bl fn_800ECA78
    lfs f1, -26364(r2)
    mr r3, r28
    bl fn_800EC9DC
    mr r3, r28
    li r4, 0x0
    bl fn_800ECB74
    mr r3, r28
    bl fn_800EC990
@L_80116C6C:
    cmplwi r29, 0x0
    beq @L_80116C7C
    mr r3, r29
    bl fn_80166A28
@L_80116C7C:
    cmpwi r31, 0x1
    beq @L_80116D10
    bge @L_80116C94
    cmpwi r31, 0x0
    bge @L_80116C9C
    b @L_80116D10
@L_80116C94:
    cmpwi r31, 0x3
    bge @L_80116D10
@L_80116C9C:
    li r3, 0x0
    li r4, 0x64
    bl fn_8018C558
    mr r0, r3
    li r3, 0x0
    mr r27, r0
    li r4, 0x65
    bl fn_8018C558
    mr r29, r3
    bl fn_8012BCA4
    cmplwi r28, 0x0
    bne @L_80116CD8
    b @L_80116CEC
    b @L_80116CD8
@L_80116CD4:
    bl _threadSwitch
@L_80116CD8:
    mr r3, r28
    bl fn_800EC960
    clrlwi r0, r3, 24
    cmplwi r0, 0x0
    bne @L_80116CD4
@L_80116CEC:
    bl fn_8012BBA8
    mr r5, r27
    li r3, 0x0
    li r4, 0x64
    bl fn_8018C8F4
    mr r5, r29
    li r3, 0x0
    li r4, 0x65
    bl fn_8018C8F4
@L_80116D10:
    li r3, 0x0
@L_80116D14:
    psq_l f31, 0x38(r1), 0, 0
    lfd f31, 0x30(r1)
    lmw r27, 0x1c(r1)
    lwz r0, 0x44(r1)
    mtlr r0
    addi r1, r1, 0x40
    blr

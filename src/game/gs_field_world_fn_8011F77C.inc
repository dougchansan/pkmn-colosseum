    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    stw r30, 0x18(r1)
    mr. r30, r3
    bne @L_8011F7A0
    li r3, 0x7
    b @L_8011F8F8
@L_8011F7A0:
    li r4, 0x0
    li r5, 0xc2
    li r6, 0x0
    bl fn_8012640C
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_8011F8F4
    mr r3, r30
    li r4, 0x0
    li r5, 0xc4
    li r6, 0x0
    bl fn_8012640C
    clrlwi. r31, r3, 16
    bne @L_8011F7DC
    li r31, 0x1
@L_8011F7DC:
    cmplwi r30, 0x0
    bne @L_8011F7EC
    lfs f2, -26288(r2)
    b @L_8011F824
@L_8011F7EC:
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
    fdivs f2, f1, f0
@L_8011F824:
    lfs f0, -26288(r2)
    fcmpo cr0, f2, f0
    bge @L_8011F838
    fmr f1, f0
    b @L_8011F860
@L_8011F838:
    clrlwi r3, r31, 16
    lis r0, 0x4330
    stw r3, 0xc(r1)
    lfs f0, -26284(r2)
    stw r0, 0x8(r1)
    lfd f1, -26256(r2)
    fmuls f2, f0, f2
    lfd f0, 0x8(r1)
    fsubs f0, f0, f1
    fdivs f1, f2, f0
@L_8011F860:
    lfs f0, -26284(r2)
    fcmpo cr0, f1, f0
    cror eq, gt, eq
    bne @L_8011F878
    li r3, 0x0
    b @L_8011F8F8
@L_8011F878:
    lfs f0, -26280(r2)
    fcmpo cr0, f1, f0
    cror eq, gt, eq
    bne @L_8011F890
    li r3, 0x1
    b @L_8011F8F8
@L_8011F890:
    lfs f0, -26276(r2)
    fcmpo cr0, f1, f0
    cror eq, gt, eq
    bne @L_8011F8A8
    li r3, 0x2
    b @L_8011F8F8
@L_8011F8A8:
    lfs f0, -26272(r2)
    fcmpo cr0, f1, f0
    cror eq, gt, eq
    bne @L_8011F8C0
    li r3, 0x3
    b @L_8011F8F8
@L_8011F8C0:
    lfs f0, -26268(r2)
    fcmpo cr0, f1, f0
    cror eq, gt, eq
    bne @L_8011F8D8
    li r3, 0x4
    b @L_8011F8F8
@L_8011F8D8:
    lfs f0, -26288(r2)
    fcmpo cr0, f1, f0
    ble @L_8011F8EC
    li r3, 0x5
    b @L_8011F8F8
@L_8011F8EC:
    li r3, 0x6
    b @L_8011F8F8
@L_8011F8F4:
    li r3, 0x7
@L_8011F8F8:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr

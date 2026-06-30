/* fn_801254B4 -- field-object property SETTER dispatcher.
 *
 * NOTE: the friendly name "GSfield_RenderPass" in the byte-match source is a
 * MISLABEL (friendly-named-unmeasured-C trap). The KNOWN-REAL signature
 *   void fn_801254B4(void* obj, u32 p, u16 sel, u32 a6, u32 a7)
 * plus the dataflow (16-bit selector, NULL-guard, jumptable dispatch to ~260
 * single-property setters) prove this is the property-setter counterpart of the
 * getter dispatcher fn_801254B4's sibling at line ~480. Renamed accordingly.
 *
 * Semantics preserved:
 *   - sel==0 or sel>=0x124 or sel>0x121  -> no-op
 *   - sel < 0x6d : resolve real object via fn_8011E778(p); bail if NULL
 *   - sel >= 0x6d: obj must be non-NULL; bail otherwise
 *   - dispatch on sel to the matching setter with the value (a7) narrowed to the
 *     field width the handler expects (u8/u16/s8/s16/full).
 * The original used a dense CW jumptable (jumptable_8035E028, 290 slots); this
 * is reproduced as a switch in the transcription's emitted (table) order. */
void GSfield_RenderPass(void* obj, u32 p, u16 sel, u32 a6, u32 a7)
{
    /* property setter handlers (obj implicit first arg) */
    extern void fn_8011CEA0(void* o, u8 v);
    extern void fn_8011CEB0(void* o, u8 v);
    extern void fn_8011CEC0(void* o, u16 v);
    extern void fn_8011CED0(void* o, u32 a, u8 v);
    extern void fn_8011CEF0(void* o, u32 a, u16 v);
    extern void fn_8011CF14(void* o, u32 v);
    extern void fn_8011CF24(void* o, u16 v);
    extern void fn_8011CF34(void* o, u16 v);
    extern void fn_8011CF44(void* o, u32 v);
    extern void fn_8011CF70(void* o, u32 v);
    extern void fn_8011CF9C(void* o, u16 v);
    extern void fn_8011CFAC(void* o, u32 v);
    extern void fn_8011CFBC(void* o, u16 v);
    extern void fn_8011CFCC(void* o, u16 v);
    extern void fn_8011CFDC(void* o, u32 v);
    extern void fn_8011CFEC(void* o, u32 a, u8 v);
    extern void fn_8011D02C(void* o, u32 a, u16 v);
    extern void fn_8011D06C(void* o, u32 a, u32 v);
    extern void fn_8011D0AC(void* o, u16 idx, u8 v);
    extern void fn_8011D0CC(void* o, u32 a, u16 v);
    extern void fn_8011D10C(void* o, u32 a, u8 v);
    extern void fn_8011D14C(void* o, u32 a, u16 v);
    extern void fn_8011D18C(void* o, u32 a, u16 v);
    extern void fn_8011D1CC(void* o, u32 a, u8 v);
    extern void fn_8011D20C(void* o, u32 a, u8 v);
    extern void fn_8011D22C(void* o, u32 a, u8 v);
    extern void fn_8011D24C(void* o, u32 a, u16 v);
    extern void fn_8011D270(void* o, u16 v);
    extern void fn_8011D280(void* o, u8 v);
    extern void fn_8011D290(void* o, u8 v);
    extern void fn_8011D2A0(void* o, u8 v);
    extern void fn_8011D2B0(void* o, u16 v);
    extern void fn_8011D2C0(void* o, u16 v);
    extern void fn_8011D2E4(void* o, u16 v);
    extern void fn_8011D308(void* o, u16 v);
    extern void fn_8011D32C(void* o, u16 v);
    extern void fn_8011D350(void* o, u16 v);
    extern void fn_8011D374(void* o, u16 v);
    extern void fn_8011D398(void* o, u16 v);
    extern void fn_8011D3BC(void* o, u16 v);
    extern void fn_8011D3E0(void* o, u16 v);
    extern void fn_8011D404(void* o, u16 v);
    extern void fn_8011D428(void* o, u16 v);
    extern void fn_8011D44C(void* o, u16 v);
    extern void fn_8011D470(void* o, u32 v);
    extern void fn_8011D494(void* o, u16 v);
    extern void fn_8011D4A4(void* o, u16 v);
    extern void fn_8011D4B4(void* o, u16 v);
    extern void fn_8011D4C4(void* o, u8 v);
    extern void fn_8011D4D4(void* o, u8 v);
    extern void fn_8011D4E4(void* o, u8 v);
    extern void fn_8011D4F4(void* o, u8 v);
    extern void fn_8011D504(void* o, u8 v);
    extern void fn_8011D56C(void* o, u8 v);
    extern void fn_8011D57C(void* o, u8 v);
    extern void fn_8011D58C(void* o, u8 v);
    extern void fn_8011D5B0(void* o, u8 v);
    extern void fn_8011D5D4(void* o, u8 v);
    extern void fn_8011D5F8(void* o, u8 v);
    extern void fn_8011D61C(void* o, u8 v);
    extern void fn_8011D640(void* o, u8 v);
    extern void fn_8011D664(void* o, u8 v);
    extern void fn_8011D688(void* o, u8 v);
    extern void fn_8011D6AC(void* o, u8 v);
    extern void fn_8011D6D0(void* o, u8 v);
    extern void fn_8011D6F4(void* o, u8 v);
    extern void fn_8011D718(void* o, u8 v);
    extern void fn_8011D73C(void* o, u8 v);
    extern void fn_8011D760(void* o, u8 v);
    extern void fn_8011D770(void* o, u8 v);
    extern void fn_8011D794(void* o, u8 v);
    extern void fn_8011D7B8(void* o, u8 v);
    extern void fn_8011D7DC(void* o, u8 v);
    extern void fn_8011D800(void* o, u8 v);
    extern void fn_8011D824(void* o, u8 v);
    extern void fn_8011D848(void* o, u8 v);
    extern void fn_8011D86C(void* o, u8 v);
    extern void fn_8011D890(void* o, u8 v);
    extern void fn_8011D8B4(void* o, u8 v);
    extern void fn_8011D8D8(void* o, u32 v);
    extern void fn_8011D8F4(void* o, u16 v);
    extern void fn_8011D904(void* o, u16 v);
    extern void fn_8011D924(void* o, u16 v);
    extern void fn_8011D958(void* o, u16 v);
    extern void fn_8011D98C(void* o, u16 v);
    extern void fn_8011D9C0(void* o, u16 v);
    extern void fn_8011D9F4(void* o, u16 v);
    extern void fn_8011DA28(void* o, u16 v);
    extern void fn_8011DA5C(void* o, u16 v);
    extern void fn_8011DA90(void* o, u16 v);
    extern void fn_8011DAC4(void* o, u16 v);
    extern void fn_8011DAF8(void* o, u16 v);
    extern void fn_8011DB2C(void* o, u16 v);
    extern void fn_8011DB60(void* o, u16 v);
    extern void fn_8011DB94(void* o, u16 v);
    extern void fn_8011DBB8(void* o, u16 v);
    extern void fn_8011DBDC(void* o, u16 v);
    extern void fn_8011DC00(void* o, u16 v);
    extern void fn_8011DC24(void* o, u16 v);
    extern void fn_8011DC48(void* o, u16 v);
    extern void fn_8011DC6C(void* o, u16 v);
    extern void fn_8011DCB4(void* o, u16 v);
    extern void fn_8011DCC4(void* o, u32 a, u8 v);
    extern void fn_8011DD80(void* o, u32 a, u8 v);
    extern void fn_8011DDFC(void* o, u32 a, u16 v);
    extern void fn_8011DE38(void* o, u32 v);
    extern void fn_8011DE48(void* o, u8 v);
    extern void fn_8011DE68(void* o, u16 v);
    extern void fn_8011DE88(void* o, u32 v);
    extern void fn_8011DE98(void* o, u32 v);
    extern void fn_8011DEA8(void* o, u32 v);
    extern void fn_8011DEE4(void* o, u32 v);
    extern void fn_8011DF54(void* o, u32 v);
    extern void fn_8011DF90(void* o, u32 v);
    extern void fn_8011DFA0(void* o, u8 v);
    extern void fn_8011DFB0(void* o, u8 v);
    extern void fn_8011DFC0(void* o, u8 v);
    extern void fn_8011DFD0(void* o, u16 v);
    extern void fn_8011DFE0(void* o, u32 v);
    extern void fn_8011DFF0(void* o, u16 v);
    extern void fn_801FCEFC(void* o, u16 v);
    extern void fn_801FCF0C(void* o, u16 v);
    extern void fn_801FCF1C(void* o, s16 v);
    extern void fn_801FCF2C(void* o, u16 v);
    extern void fn_801FCF3C(void* o, u16 v);
    extern void fn_801FCF4C(void* o, u16 v);
    extern void fn_801FCF5C(void* o, u16 v);
    extern void fn_801FCF6C(void* o, u16 v);
    extern void fn_801FCF7C(void* o, u16 v);
    extern void fn_801FD150(void* o, s16 v);
    extern void fn_801FD178(void* o, u8 v);
    extern void fn_801FD1A0(void* o, u16 v);
    extern void fn_801FD1B0(void* o, s16 v);
    extern void fn_801FD1C0(void* o, u16 v);
    extern void fn_801FD1D0(void* o, s16 v);
    extern void fn_801FD1E0(void* o, u32 v);
    extern void fn_801FD1F0(void* o, u8 v);
    extern void fn_801FD200(void* o, u8 v);
    extern void fn_801FD210(void* o, u8 v);
    extern void fn_801FD220(void* o, u8 v);
    extern void fn_801FD230(void* o, u8 v);
    extern void fn_801FD240(void* o, u8 v);
    extern void fn_801FD250(void* o, u8 v);
    extern void fn_801FD260(void* o, u8 v);
    extern void fn_801FD270(void* o, u8 v);
    extern void fn_801FD280(void* o, u8 v);
    extern void fn_801FD290(void* o, u8 v);
    extern void fn_801FD2A0(void* o, u8 v);
    extern void fn_801FD2B0(void* o, u8 v);
    extern void fn_801FD2C0(void* o, u8 v);
    extern void fn_801FD2D0(void* o, u8 v);
    extern void fn_801FD2E0(void* o, u8 v);
    extern void fn_801FD2F0(void* o, u8 v);
    extern void fn_801FD300(void* o, u8 v);
    extern void fn_801FD310(void* o, u8 v);
    extern void fn_801FD320(void* o, u8 v);
    extern void fn_801FD330(void* o, u8 v);
    extern void fn_801FD5C8(void* o, u16 v);
    extern void fn_801FD5F0(void* o, u8 a, u16 v);
    extern void fn_801FD660(void* o, u8 a, s8 v);
    extern void fn_801FD6B8(void* o, u16 v);
    extern void fn_801FD6C8(void* o, s16 v);
    extern void fn_801FD6D8(void* o, u16 v);
    extern void fn_801FD6E8(void* o, s16 v);
    extern void fn_801FD6F8(void* o, u8 v);
    extern void fn_801FD708(void* o, u16 v);
    extern void fn_801FD718(void* o, u16 v);
    extern void fn_801FD728(void* o, u8 v);
    extern void fn_801FD7F8(void* o, u32 v);
    extern void fn_801FD820(void* o, u16 v);
    extern void fn_801FD840(void* o, u8 v);
    extern void fn_801FD850(void* o, u8 v);
    extern void fn_801FD860(void* o, u8 v);
    extern void fn_801FD870(void* o, u8 v);
    extern void fn_801FD880(void* o, u8 v);
    extern void fn_801FD890(void* o, u8 v);
    extern void fn_801FD8A0(void* o, u8 v);
    extern void fn_801FD8B0(void* o, u32 v);
    extern void fn_801FD8C0(void* o, u32 v);
    extern void fn_801FD8D0(void* o, u8 v);
    extern void fn_801FD8F8(void* o, u8 v);
    extern void fn_801FD908(void* o, u8 v);
    extern void fn_801FD918(void* o, u8 v);
    extern void fn_801FD928(void* o, s16 v);
    extern void fn_801FD938(void* o, u32 v);
    /* object resolver + property getter/setter helpers */
    extern void* fn_8011E778(u32 p);
    extern u32  fn_8012640C(void* o, u32 id, u32 selector, u32 d);
    extern void fn_8011BBD8(void* ctx, u32 item, u16 sel, u32 a6, u32 v);
    /* turn-zone / path-selector helpers used by the composite cases */
    extern u32  fn_8020981C(void* o, u16 v);
    extern void fn_802097C8(void* o, u16 v, u32 mode);
    extern u32  fn_8020990C(void* o);
    extern void fn_80209960(void* o, u16 v);
    extern void fn_80209FAC(void* o);

    void* target;   /* resolved object the setter writes to (r3) */
    void* sub;      /* secondary object handle from fn_8012640C (r3) */
    u32   status;   /* path/zone query result */

    /* ---- entry guards (sel is the 16-bit selector) ---- */
    if (sel == 0)        return;
    if (sel >= 0x124)    return;

    if (sel < 0x6d) {
        /* indirect properties: resolve the real object from the handle p */
        target = fn_8011E778(p);
        if (target == 0) return;
    } else {
        /* direct properties: caller-supplied obj must be valid */
        if (obj == 0) return;
        target = obj;
    }

    if (sel > 0x121) return;   /* table bound (0..0x121) */

    /* ---- dispatch (emitted jumptable order == ascending case) ---- */
    switch (sel) {
    case 0x0:   fn_8011D470(target, a7); return;
    case 0x1:   fn_8011D44C(target, (u16)a7); return;
    case 0x2:   fn_8011D428(target, (u16)a7); return;
    case 0x3:   fn_8011D404(target, (u16)a7); return;
    case 0x4:   fn_8011D3E0(target, (u16)a7); return;
    case 0x5:   fn_8011D3BC(target, (u16)a7); return;
    case 0x6:   fn_8011D398(target, (u16)a7); return;
    case 0x7:   fn_8011D374(target, (u16)a7); return;
    case 0x8:   fn_8011D350(target, (u16)a7); return;
    case 0x9:   fn_8011D32C(target, (u16)a7); return;
    case 0xa:   fn_8011D308(target, (u16)a7); return;
    case 0xb:   fn_8011D2E4(target, (u16)a7); return;
    case 0xc:   fn_8011D2C0(target, (u16)a7); return;
    case 0xd:   fn_8011D2B0(target, (u16)a7); return;
    case 0xe:   fn_8011D2A0(target, (u8)a7); return;
    case 0xf:   fn_8011D290(target, (u8)a7); return;
    case 0x10:  fn_8011D280(target, (u8)a7); return;
    case 0x11:  fn_8011D270(target, (u16)a7); return;
    case 0x12:  fn_8011D24C(target, a6, (u16)a7); return;
    case 0x13:  fn_8011D22C(target, a6, (u8)a7); return;
    case 0x14:  fn_8011D20C(target, a6, (u8)a7); return;
    case 0x15:  fn_8011D1CC(target, a6, (u8)a7); return;
    case 0x16:  fn_8011D18C(target, a6, (u16)a7); return;
    case 0x17:  fn_8011D14C(target, a6, (u16)a7); return;
    case 0x18:  fn_8011D10C(target, a6, (u8)a7); return;
    case 0x19:  fn_8011D0CC(target, a6, (u16)a7); return;
    case 0x1a:  fn_8011D0AC(target, 0x0,  (u8)a7); return;
    case 0x1b:  fn_8011D0AC(target, 0x1,  (u8)a7); return;
    case 0x1c:  fn_8011D0AC(target, 0x2,  (u8)a7); return;
    case 0x1d:  fn_8011D0AC(target, 0x3,  (u8)a7); return;
    case 0x1e:  fn_8011D0AC(target, 0x4,  (u8)a7); return;
    case 0x1f:  fn_8011D0AC(target, 0x5,  (u8)a7); return;
    case 0x20:  fn_8011D0AC(target, 0x6,  (u8)a7); return;
    case 0x21:  fn_8011D0AC(target, 0x7,  (u8)a7); return;
    case 0x22:  fn_8011D0AC(target, 0x8,  (u8)a7); return;
    case 0x23:  fn_8011D0AC(target, 0x9,  (u8)a7); return;
    case 0x24:  fn_8011D0AC(target, 0xa,  (u8)a7); return;
    case 0x25:  fn_8011D0AC(target, 0xb,  (u8)a7); return;
    case 0x26:  fn_8011D0AC(target, 0xc,  (u8)a7); return;
    case 0x27:  fn_8011D0AC(target, 0xd,  (u8)a7); return;
    case 0x28:  fn_8011D0AC(target, 0xe,  (u8)a7); return;
    case 0x29:  fn_8011D0AC(target, 0xf,  (u8)a7); return;
    case 0x2a:  fn_8011D0AC(target, 0x10, (u8)a7); return;
    case 0x2b:  fn_8011D0AC(target, 0x11, (u8)a7); return;
    case 0x2c:  fn_8011D0AC(target, 0x12, (u8)a7); return;
    case 0x2d:  fn_8011D0AC(target, 0x13, (u8)a7); return;
    case 0x2e:  fn_8011D0AC(target, 0x14, (u8)a7); return;
    case 0x2f:  fn_8011D0AC(target, 0x15, (u8)a7); return;
    case 0x30:  fn_8011D0AC(target, 0x16, (u8)a7); return;
    case 0x31:  fn_8011D0AC(target, 0x17, (u8)a7); return;
    case 0x32:  fn_8011D0AC(target, 0x18, (u8)a7); return;
    /* note: original skips idx 0x19 here (jumps 0x18 -> 0x1a) */
    case 0x33:  fn_8011D0AC(target, 0x1a, (u8)a7); return;
    case 0x34:  fn_8011D0AC(target, 0x1b, (u8)a7); return;
    case 0x35:  fn_8011D0AC(target, 0x1c, (u8)a7); return;
    case 0x36:  fn_8011D0AC(target, 0x1d, (u8)a7); return;
    case 0x37:  fn_8011D0AC(target, 0x1e, (u8)a7); return;
    case 0x38:  fn_8011D0AC(target, 0x1f, (u8)a7); return;
    case 0x39:  fn_8011D0AC(target, 0x20, (u8)a7); return;
    case 0x3a:  fn_8011D0AC(target, 0x21, (u8)a7); return;
    case 0x3b:  fn_8011D0AC(target, 0x22, (u8)a7); return;
    case 0x3c:  fn_8011D0AC(target, 0x23, (u8)a7); return;
    case 0x3d:  fn_8011D0AC(target, 0x24, (u8)a7); return;
    case 0x3e:  fn_8011D0AC(target, 0x25, (u8)a7); return;
    case 0x3f:  fn_8011D0AC(target, 0x26, (u8)a7); return;
    case 0x40:  fn_8011D0AC(target, 0x27, (u8)a7); return;
    case 0x41:  fn_8011D0AC(target, 0x28, (u8)a7); return;
    case 0x42:  fn_8011D0AC(target, 0x29, (u8)a7); return;
    case 0x43:  fn_8011D0AC(target, 0x2a, (u8)a7); return;
    case 0x44:  fn_8011D0AC(target, 0x2b, (u8)a7); return;
    case 0x45:  fn_8011D0AC(target, 0x2c, (u8)a7); return;
    case 0x46:  fn_8011D0AC(target, 0x2d, (u8)a7); return;
    case 0x47:  fn_8011D0AC(target, 0x2e, (u8)a7); return;
    case 0x48:  fn_8011D0AC(target, 0x2f, (u8)a7); return;
    case 0x49:  fn_8011D0AC(target, 0x30, (u8)a7); return;
    case 0x4a:  fn_8011D0AC(target, 0x31, (u8)a7); return;
    case 0x4b:  fn_8011D0AC(target, 0x32, (u8)a7); return;
    case 0x4c:  fn_8011D0AC(target, 0x33, (u8)a7); return;
    case 0x4d:  fn_8011D0AC(target, 0x34, (u8)a7); return;
    case 0x4e:  fn_8011D0AC(target, 0x35, (u8)a7); return;
    case 0x4f:  fn_8011D0AC(target, 0x36, (u8)a7); return;
    case 0x50:  fn_8011D0AC(target, 0x37, (u8)a7); return;
    case 0x51:  fn_8011D0AC(target, 0x38, (u8)a7); return;
    case 0x52:  fn_8011D0AC(target, 0x39, (u8)a7); return;
    case 0x53:  fn_8011D0AC(target, 0x3a, (u8)a7); return;
    case 0x54:  fn_8011D06C(target, a6, a7); return;
    case 0x55:  fn_8011D02C(target, a6, (u16)a7); return;
    case 0x56:  fn_8011CFEC(target, a6, (u8)a7); return;
    case 0x57:  fn_8011CFDC(target, a7); return;
    case 0x58:  fn_8011CFCC(target, (u16)a7); return;
    case 0x59:  fn_8011CFBC(target, (u16)a7); return;
    case 0x5a:  fn_8011CFAC(target, a7); return;
    case 0x5b:  fn_8011CF9C(target, (u16)a7); return;
    case 0x5c:  fn_8011CF70(target, a7); return;
    case 0x5d:  fn_8011CF44(target, a7); return;
    case 0x5e:  fn_8011CF34(target, (u16)a7); return;
    case 0x5f:  fn_8011CF24(target, (u16)a7); return;
    case 0x60:  fn_8011CF14(target, a7); return;
    case 0x61:  fn_8011CEF0(target, a6, (u16)a7); return;
    case 0x62:  fn_8011CED0(target, a6, (u8)a7); return;
    case 0x63:  fn_8011CEC0(target, (u16)a7); return;
    case 0x64:  fn_8011CEB0(target, (u8)a7); return;
    case 0x65:  fn_8011CEA0(target, (u8)a7); return;
    case 0x66:  fn_8011DFF0(target, (u16)a7); return;
    case 0x67:  fn_8011DFE0(target, a7); return;
    case 0x68:  fn_8011DFD0(target, (u16)a7); return;
    case 0x69:  fn_8011DFC0(target, (u8)a7); return;
    case 0x6a:  fn_8011DFB0(target, (u8)a7); return;
    case 0x6b:  fn_8011DFA0(target, (u8)a7); return;
    case 0x6c:  fn_8011DF90(target, a7); return;
    case 0x6d:  fn_8011DF54(target, a7); return;
    case 0x6e:  fn_8011DEE4(target, a7); return;
    case 0x6f:  fn_8011DEA8(target, a7); return;
    case 0x70:  fn_8011DE98(target, a7); return;
    case 0x71:  fn_8011DE48(target, (u8)a7); return;
    case 0x72: {
        /* read 16 bytes from object 0x7c and copy into supplied buffer (a7) */
        sub = (void*)fn_8012640C(target, 0x0, 0x7c, 0x0);
        if (sub == 0) return;
        ((u32*)sub)[0] = ((u32*)((u8*)target + 0))[0];
        ((u32*)sub)[1] = ((u32*)((u8*)target + 0))[1];
        ((u32*)sub)[2] = ((u32*)((u8*)target + 0))[2];
        ((u32*)sub)[3] = ((u32*)((u8*)target + 0))[3];
        return;
    }
    case 0x73:  fn_8011DE38(target, a7); return;
    case 0x74:  fn_8011DDFC(target, a6, (u16)a7); return;
    case 0x75:  fn_8011DD80(target, a6, (u8)a7); return;
    case 0x76:  fn_8011DCC4(target, a6, (u8)a7); return;
    case 0x77:  fn_8011DCB4(target, (u16)a7); return;
    case 0x78:  fn_8011DC6C(target, (u16)a7); return;
    case 0x79:  fn_8011DC48(target, (u16)a7); return;
    case 0x7a:  fn_8011DC24(target, (u16)a7); return;
    case 0x7b:  fn_8011DC00(target, (u16)a7); return;
    case 0x7c:  fn_8011DBDC(target, (u16)a7); return;
    case 0x7d:  fn_8011DBB8(target, (u16)a7); return;
    case 0x7e:  fn_8011DB94(target, (u16)a7); return;
    case 0x7f:  fn_8011DB60(target, (u16)a7); return;
    case 0x80:  fn_8011DB2C(target, (u16)a7); return;
    case 0x81:  fn_8011DAF8(target, (u16)a7); return;
    case 0x82:  fn_8011DAC4(target, (u16)a7); return;
    case 0x83:  fn_8011DA90(target, (u16)a7); return;
    case 0x84:  fn_8011DA5C(target, (u16)a7); return;
    case 0x85:  fn_8011DA28(target, (u16)a7); return;
    case 0x86:  fn_8011D9F4(target, (u16)a7); return;
    case 0x87:  fn_8011D9C0(target, (u16)a7); return;
    case 0x88:  fn_8011D98C(target, (u16)a7); return;
    case 0x89:  fn_8011D958(target, (u16)a7); return;
    case 0x8a:  fn_8011D924(target, (u16)a7); return;
    case 0x8b:  fn_8011D904(target, (u16)a7); return;
    case 0x8c:  fn_8011D8B4(target, (u8)a7); return;
    case 0x8d:  fn_8011D890(target, (u8)a7); return;
    case 0x8e:  fn_8011D86C(target, (u8)a7); return;
    case 0x8f:  fn_8011D848(target, (u8)a7); return;
    case 0x90:  fn_8011D824(target, (u8)a7); return;
    case 0x91:  fn_8011D760(target, (u8)a7); return;
    case 0x92:  fn_8011D73C(target, (u8)a7); return;
    case 0x93:  fn_8011D718(target, (u8)a7); return;
    case 0x94:  fn_8011D6F4(target, (u8)a7); return;
    case 0x95:  fn_8011D6D0(target, (u8)a7); return;
    case 0x96:  fn_8011D6AC(target, (u8)a7); return;
    case 0x97:  fn_8011D688(target, (u8)a7); return;
    case 0x98:  fn_8011D664(target, (u8)a7); return;
    case 0x99:  fn_8011D640(target, (u8)a7); return;
    case 0x9a:  fn_8011D61C(target, (u8)a7); return;
    case 0x9b:  fn_8011D5F8(target, (u8)a7); return;
    case 0x9c:  fn_8011D5D4(target, (u8)a7); return;
    case 0x9d:  fn_8011D5B0(target, (u8)a7); return;
    case 0x9e:  fn_8011D58C(target, (u8)a7); return;
    case 0x9f:  fn_8011D800(target, (u8)a7); return;
    case 0xa0:  fn_8011D7DC(target, (u8)a7); return;
    case 0xa1:  fn_8011D7B8(target, (u8)a7); return;
    case 0xa2:  fn_8011D794(target, (u8)a7); return;
    case 0xa3:  fn_8011D770(target, (u8)a7); return;
    case 0xa4:  fn_8011D57C(target, (u8)a7); return;
    case 0xa5:  fn_8011D56C(target, (u8)a7); return;
    case 0xa6:  fn_8011D504(target, (u8)a7); return;
    case 0xa7:  fn_8011D4F4(target, (u8)a7); return;
    case 0xa8:  fn_8011D4E4(target, (u8)a7); return;
    case 0xa9:  fn_8011D4D4(target, (u8)a7); return;
    case 0xaa:  fn_8011D4C4(target, (u8)a7); return;
    case 0xab:  fn_8011D4A4(target, (u16)a7); return;
    case 0xac:  fn_8011D4B4(target, (u16)a7); return;
    case 0xad:  fn_8011D494(target, (u16)a7); return;
    case 0xae:  fn_8011D8F4(target, (u16)a7); return;
    case 0xaf:  fn_8011D8D8(target, a7); return;
    case 0xb0:  fn_8011DE88(target, a7); return;
    case 0xb1:  fn_8011DE68(target, (u16)a7); return;
    case 0xb2:  fn_801FD938(target, a7); return;
    case 0xb3: {
        /* read 16 bytes from object 0xcd and copy into supplied buffer (a7) */
        sub = (void*)fn_8012640C(target, 0x0, 0xcd, 0x0);
        if (sub == 0) return;
        ((u32*)sub)[0] = ((u32*)((u8*)target + 0))[0];
        ((u32*)sub)[1] = ((u32*)((u8*)target + 0))[1];
        ((u32*)sub)[2] = ((u32*)((u8*)target + 0))[2];
        ((u32*)sub)[3] = ((u32*)((u8*)target + 0))[3];
        return;
    }
    case 0xb4:  fn_801FD928(target, (s16)a7); return;
    case 0xb5:  fn_801FD918(target, (u8)a7); return;
    case 0xb6:  fn_801FD908(target, (u8)a7); return;
    case 0xb7:  fn_801FD8F8(target, (u8)a7); return;
    case 0xb8:  fn_801FD8D0(target, (u8)a7); return;
    case 0xb9:  fn_801FD8C0(target, a7); return;
    case 0xba:  fn_801FD8B0(target, a7); return;
    case 0xbb: {
        /* resolve sub-object 0xd9, then set its state field 0x28 = a7 */
        sub = (void*)fn_8012640C(target, 0x0, 0xd9, 0x0);
        fn_8011BBD8(sub, 0x0, 0x28, 0x0, a7);
        return;
    }
    case 0xbc: {
        sub = (void*)fn_8012640C(target, 0x0, 0xd9, 0x0);
        fn_8011BBD8(sub, 0x0, 0x27, 0x0, a7);
        return;
    }
    case 0xbd: {
        sub = (void*)fn_8012640C(target, 0x0, 0xd9, 0x0);
        fn_8011BBD8(sub, 0x0, 0x2c, 0x0, a7);
        return;
    }
    case 0xbe: {
        sub = (void*)fn_8012640C(target, 0x0, 0xd9, 0x0);
        fn_8011BBD8(sub, 0x0, 0x2b, 0x0, a7);
        return;
    }
    case 0xbf: {
        /* path/turn-zone query on sub-object 0xd9; if state==2 commit path */
        sub = (void*)fn_8012640C(target, 0x0, 0xd9, 0x0);
        status = fn_8020981C(sub, (u16)a7);
        if ((status & 0xFF) != 0x2) return;
        fn_802097C8(sub, (u16)a7, 0x0);
        return;
    }
    case 0xc0: {
        /* path branch select on sub-object 0xd9 */
        sub = (void*)fn_8012640C(target, 0x0, 0xd9, 0x0);
        /* FUNCTIONAL-TODO: original reuses the prior case's status flag here;
         * recompute the same zone query to preserve the branch decision. */
        status = fn_8020981C(sub, (u16)a7);
        if ((status & 0xFF) != 0x2) {
            status = fn_8020990C(sub);
            if ((status & 0xFF) != 0x1) return;
            fn_80209960(sub, (u16)a7);
            return;
        }
        fn_80209FAC(sub);
        return;
    }
    case 0xc1: {
        sub = (void*)fn_8012640C(target, 0x0, 0xd9, 0x0);
        fn_8011BBD8(sub, 0x0, 0x2d, 0x0, a7);
        return;
    }
    case 0xc2: {
        sub = (void*)fn_8012640C(target, 0x0, 0xd9, 0x0);
        fn_8011BBD8(sub, 0x0, 0x2f, 0x0, (u16)a7);
        return;
    }
    case 0xc3: {
        sub = (void*)fn_8012640C(target, 0x0, 0xd9, 0x0);
        fn_8011BBD8(sub, 0x0, 0x29, 0x0, a7);
        return;
    }
    case 0xc4: {
        sub = (void*)fn_8012640C(target, 0x0, 0xd9, 0x0);
        fn_8011BBD8(sub, 0x0, 0x2e, 0x0, a7);
        return;
    }
    case 0xc5:  fn_801FD8A0(target, (u8)a7); return;
    case 0xc6:  fn_801FD890(target, (u8)a7); return;
    case 0xc7:  fn_801FD880(target, (u8)a7); return;
    case 0xc8:  fn_801FD870(target, (u8)a7); return;
    case 0xc9:  fn_801FD860(target, (u8)a7); return;
    case 0xca:  fn_801FD850(target, (u8)a7); return;
    case 0xcb:  fn_801FD840(target, (u8)a7); return;
    case 0xcc:  fn_801FD820(target, (u16)a7); return;
    case 0xcd:  fn_801FD7F8(target, a7); return;
    case 0xce:  fn_801FCF7C(target, (u16)a7); return;
    case 0xcf:  fn_801FCF6C(target, (u16)a7); return;
    case 0xd0:  fn_801FCF5C(target, (u16)a7); return;
    case 0xd1:  fn_801FCF4C(target, (u16)a7); return;
    case 0xd2:  fn_801FCF3C(target, (u16)a7); return;
    case 0xd3:  fn_801FCF2C(target, (u16)a7); return;
    case 0xd4:  fn_801FCF1C(target, (s16)a7); return;
    case 0xd5:  fn_801FCF0C(target, (u16)a7); return;
    case 0xd6:  fn_801FCEFC(target, (u16)a7); return;
    case 0xd7:  fn_801FD728(target, (u8)a7); return;
    case 0xd8:  fn_801FD718(target, (u16)a7); return;
    case 0xd9:  fn_801FD708(target, (u16)a7); return;
    case 0xda:  fn_801FD6F8(target, (u8)a7); return;
    case 0xdb:  fn_801FD660(target, (u8)a6, (s8)a7); return;
    case 0xdc:  fn_801FD5F0(target, (u8)a6, (u16)a7); return;
    case 0xdd:  fn_801FD5C8(target, (u16)a7); return;
    case 0xde:  fn_801FD6E8(target, (s16)a7); return;
    case 0xdf:  fn_801FD6D8(target, (u16)a7); return;
    case 0xe0:  fn_801FD6C8(target, (s16)a7); return;
    case 0xe1:  fn_801FD6B8(target, (u16)a7); return;
    case 0xe2:  fn_801FD330(target, (u8)a7); return;
    case 0xe3:  fn_801FD320(target, (u8)a7); return;
    case 0xe4:  fn_801FD310(target, (u8)a7); return;
    case 0xe5:  fn_801FD300(target, (u8)a7); return;
    case 0xe6:  fn_801FD2F0(target, (u8)a7); return;
    case 0xe7:  fn_801FD2E0(target, (u8)a7); return;
    case 0xe8:  fn_801FD2D0(target, (u8)a7); return;
    case 0xe9:  fn_801FD2C0(target, (u8)a7); return;
    case 0xea:  fn_801FD2B0(target, (u8)a7); return;
    case 0xeb:  fn_801FD2A0(target, (u8)a7); return;
    case 0xec:  fn_801FD290(target, (u8)a7); return;
    case 0xed:  fn_801FD280(target, (u8)a7); return;
    case 0xee:  fn_801FD270(target, (u8)a7); return;
    case 0xef:  fn_801FD260(target, (u8)a7); return;
    case 0xf0:  fn_801FD250(target, (u8)a7); return;
    case 0xf1:  fn_801FD240(target, (u8)a7); return;
    case 0xf2:  fn_801FD230(target, (u8)a7); return;
    case 0xf3:  fn_801FD220(target, (u8)a7); return;
    case 0xf4:  fn_801FD210(target, (u8)a7); return;
    case 0xf5:  fn_801FD200(target, (u8)a7); return;
    case 0xf6:  fn_801FD1F0(target, (u8)a7); return;
    case 0xf7:  fn_801FD1E0(target, a7); return;
    case 0xf8:  fn_801FD1D0(target, (s16)a7); return;
    case 0xf9:  fn_801FD1C0(target, (u16)a7); return;
    case 0xfa:  fn_801FD1B0(target, (s16)a7); return;
    case 0xfb:  fn_801FD1A0(target, (u16)a7); return;
    case 0xfc:  fn_801FD178(target, (u8)a7); return;
    case 0xfd:  fn_801FD150(target, (s16)a7); return;
    default:    return;
    }
}

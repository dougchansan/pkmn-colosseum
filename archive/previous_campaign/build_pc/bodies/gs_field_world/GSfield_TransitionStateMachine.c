extern u32 fn_8011CE44(u32 mapId, u8 idx);
extern u32 fn_8011CE74(u32 mapId);
extern u32 fn_80131574(s32 idx);
/* GSfield_TransitionStateMachine (orig fn_8012640C) -- field/Pokemon property
 * accessor + transition dispatcher. Args: obj = field/Pokemon context object
 * (r3), id = sub-index/slot passed to some accessors (r4), selector = which
 * property/operation (r5, 16-bit), d = extra dword (r6, unused by most paths).
 * Returns the selected property value (zero-extended/sign-extended to the
 * field's natural width) or 0 for unknown/guarded selectors.
 *
 * This is a faithful functional port of the PowerPC jump-table dispatch at
 * 0x8012640C (jumptable_8035E4B0, 0x124 entries). The exact selector->handler
 * map was recovered from the original DOL; selectors not listed fall through to
 * the default (return 0), matching the table's default slots. */
u32 GSfield_TransitionStateMachine(u8 *obj, u32 id, u16 selector, u32 d)
{
    /* Property/state getters (return value already in the natural width). */
    extern u16 fn_8011E760(u8 *o);
    extern u16 fn_8011E734(u8 *o);
    extern u16 fn_8011E708(u8 *o);
    extern u16 fn_8011E6DC(u8 *o);
    extern u16 fn_8011E6B0(u8 *o);
    extern u16 fn_8011E684(u8 *o);
    extern u16 fn_8011E658(u8 *o);
    extern u16 fn_8011E62C(u8 *o);
    extern u16 fn_8011E600(u8 *o);
    extern u16 fn_8011E5D4(u8 *o);
    extern u16 fn_8011E5A8(u8 *o);
    extern u16 fn_8011E57C(u8 *o);
    extern u16 fn_8011E550(u8 *o);
    extern u16 fn_8011E538(u8 *o);
    extern u8  fn_8011E520(u8 *o);
    extern u8  fn_8011E508(u8 *o);
    extern u8  fn_8011E4F0(u8 *o);
    extern u16 fn_8011E4D8(u8 *o);
    extern u16 fn_8011E4A4(u8 *o, u32 i);
    extern u8  fn_8011E474(u8 *o, u32 i);
    extern u8  fn_8011E444(u8 *o, u32 i);
    extern u8  fn_8011E3FC(u8 *o, u32 i);
    extern u16 fn_8011E3B4(u8 *o, u32 i);
    extern u16 fn_8011E36C(u8 *o, u32 i);
    extern u8  fn_8011E324(u8 *o, u32 i);
    extern u16 fn_8011E2DC(u8 *o, u32 i);
    extern u8  fn_8011E2AC(u8 *o, u32 which);   /* indexed sub-property getter */
    extern u32 fn_8011E264(u8 *o, u32 i);
    extern u16 fn_8011E21C(u8 *o, u32 i);
    extern u8  fn_8011E1D4(u8 *o, u32 i);
    extern u32 fn_8011E1BC(u8 *o);
    extern u16 fn_8011E1A4(u8 *o);
    extern u16 fn_8011E18C(u8 *o);
    extern u32 fn_8011E174(u8 *o);
    extern u16 fn_8011E15C(u8 *o);
    extern u32 fn_8011E128(u8 *o);
    extern u32 fn_8011E0F4(u8 *o);
    extern u16 fn_8011E0DC(u8 *o);
    extern u16 fn_8011E0C4(u8 *o);
    extern u32 fn_8011E0AC(u8 *o);
    extern u16 fn_8011E078(u8 *o, u32 i);
    extern u8  fn_8011E048(u8 *o, u32 i);
    extern u16 fn_8011E030(u8 *o);
    extern u8  fn_8011E018(u8 *o);
    extern u8  fn_8011E000(u8 *o);
    extern void fn_8011E778(u32 i);             /* preamble notify/refresh hook */
    extern u16 fn_8011F5C8(u8 *o);
    extern u32 fn_8011F5B0(u8 *o);
    extern u32 fn_8011F598(u8 *o);
    extern u16 fn_8011F580(u8 *o);
    extern u8  fn_8011F568(u8 *o);
    extern u8  fn_8011F550(u8 *o);
    extern u8  fn_8011F538(u8 *o);
    extern u32 fn_8011F520(u8 *o);
    extern u32 fn_8011F508(u8 *o);
    extern u32 fn_8011F4F0(u8 *o);
    extern u32 fn_8011F4D8(u8 *o);
    extern u32 fn_8011F4C0(u8 *o);
    extern u8  fn_8011F4A8(u8 *o);
    extern u32 fn_8011F474(u8 *o, u32 i);
    extern u32 fn_8011F45C(u8 *o);
    extern u16 fn_8011F228(u8 *o, u32 i);
    extern u8  fn_8011F1F0(u8 *o, u32 i);
    extern u8  fn_8011F1B8(u8 *o, u32 i);
    extern u16 fn_8011F1A0(u8 *o);
    extern u16 fn_8011F188(u8 *o);
    extern u16 fn_8011F15C(u8 *o);
    extern u16 fn_8011F130(u8 *o);
    extern u16 fn_8011F104(u8 *o);
    extern u16 fn_8011F0D8(u8 *o);
    extern u16 fn_8011F0AC(u8 *o);
    extern u16 fn_8011F080(u8 *o);
    extern u16 fn_8011F054(u8 *o);
    extern u16 fn_8011F028(u8 *o);
    extern u16 fn_8011EFFC(u8 *o);
    extern u16 fn_8011EFD0(u8 *o);
    extern u16 fn_8011EFA4(u8 *o);
    extern u16 fn_8011EF78(u8 *o);
    extern u16 fn_8011EF4C(u8 *o);
    extern u16 fn_8011EF20(u8 *o);
    extern u16 fn_8011EEF4(u8 *o);
    extern u16 fn_8011EEC8(u8 *o);
    extern u16 fn_8011EE9C(u8 *o);
    extern u16 fn_8011EE70(u8 *o);
    extern u16 fn_8011EE58(u8 *o);
    extern u8  fn_8011ECEC(u8 *o);
    extern u8  fn_8011ECC0(u8 *o);
    extern u8  fn_8011EC94(u8 *o);
    extern u8  fn_8011EC68(u8 *o);
    extern u8  fn_8011EC3C(u8 *o);
    extern u8  fn_8011EB48(u8 *o);
    extern u8  fn_8011EB1C(u8 *o);
    extern u8  fn_8011EAF0(u8 *o);
    extern u8  fn_8011EAC4(u8 *o);
    extern u8  fn_8011EA98(u8 *o);
    extern u8  fn_8011EA6C(u8 *o);
    extern u8  fn_8011EA40(u8 *o);
    extern u8  fn_8011EA14(u8 *o);
    extern u8  fn_8011E9E8(u8 *o);
    extern u8  fn_8011E9BC(u8 *o);
    extern u8  fn_8011E990(u8 *o);
    extern u8  fn_8011E964(u8 *o);
    extern u8  fn_8011E938(u8 *o);
    extern u8  fn_8011E90C(u8 *o);
    extern u8  fn_8011EC10(u8 *o);
    extern u8  fn_8011EBE4(u8 *o);
    extern u8  fn_8011EBB8(u8 *o);
    extern u8  fn_8011EB8C(u8 *o);
    extern u8  fn_8011EB60(u8 *o);
    extern u8  fn_8011E8F4(u8 *o);
    extern u8  fn_8011E8DC(u8 *o);
    extern u8  fn_8011E868(u8 *o);
    extern u8  fn_8011E850(u8 *o);
    extern u8  fn_8011E838(u8 *o);
    extern u8  fn_8011E820(u8 *o);
    extern u8  fn_8011E808(u8 *o);
    extern u16 fn_8011E7D8(u8 *o);
    extern u16 fn_8011E7F0(u8 *o);
    extern u16 fn_8011E7C0(u8 *o);
    extern u16 fn_8011EE40(u8 *o);
    extern u32 fn_8011ED18(u8 *o);
    extern u32 fn_8011EE28(u8 *o);
    extern u32 fn_8011EE10(u8 *o);
    extern u16 fn_8011EDF8(u8 *o);
    extern u8  fn_8011ED68(u8 *o);
    extern u32 fn_8011EDC4(u8 *o, u32 i);
    extern u32 fn_801FDB60(u8 *o);
    extern u32 fn_801FDB48(u8 *o);
    extern u32 fn_801FDB14(u8 *o, u32 i);
    extern s16 fn_801FDAFC(u8 *o);
    extern u8  fn_801FDAE4(u8 *o);
    extern u8  fn_801FDACC(u8 *o);
    extern u8  fn_801FDAB4(u8 *o);
    extern u8  fn_801FD8E0(u8 *o);
    extern u32 fn_801FDA9C(u8 *o);
    extern u32 fn_801FDA84(u8 *o);
    extern u32 fn_801FDA6C(u8 *o);
    extern u32 fn_801FDA38(u8 *o, u32 i);
    extern u32 fn_801FDA20(u8 *o);
    extern u16 fn_80205184(u8 *o);
    extern u16 fn_80205224(u8 *o);
    extern u32 fn_8011BEB4(void *ctx, u32 item, u16 field, u32 flags);
    extern u8  fn_8020990C(void *ctx, u32 v);
    extern u8  fn_802096E8(void *ctx);
    extern u32 fn_801FDA08(u8 *o);
    extern u8  fn_801FD9F0(u8 *o);
    extern u8  fn_801FD9D8(u8 *o);
    extern u8  fn_801FD9C0(u8 *o);
    extern u8  fn_801FD9A8(u8 *o);
    extern u8  fn_801FD990(u8 *o);
    extern u8  fn_801FD978(u8 *o);
    extern u8  fn_801FD960(u8 *o);
    extern u16 fn_801FD948(u8 *o);
    extern u32 fn_801FD808(u8 *o);
    extern u16 fn_801FD064(u8 *o);
    extern u16 fn_801FD04C(u8 *o);
    extern u16 fn_801FD034(u8 *o);
    extern u16 fn_801FD01C(u8 *o);
    extern u16 fn_801FD004(u8 *o);
    extern u16 fn_801FCFEC(u8 *o);
    extern s16 fn_801FCFD4(u8 *o);
    extern u16 fn_801FCFBC(u8 *o);
    extern u16 fn_801FCFA4(u8 *o);
    extern u32 fn_801FCF8C(u8 *o);
    extern u8  fn_801FD7E0(u8 *o);
    extern u16 fn_801FD7C8(u8 *o);
    extern u16 fn_801FD7B0(u8 *o);
    extern u8  fn_801FD798(u8 *o);
    extern s16 fn_801FD684(u8 *o, u8 i);
    extern u32 fn_801FD648(u8 *o);
    extern u16 fn_801FD614(u8 *o, u8 i);
    extern u16 fn_801FD5D8(u8 *o);
    extern u32 fn_801FD5B0(u8 *o);
    extern s16 fn_801FD780(u8 *o);
    extern u16 fn_801FD768(u8 *o);
    extern s16 fn_801FD750(u8 *o);
    extern u16 fn_801FD738(u8 *o);
    extern u8  fn_801FD598(u8 *o);
    extern u8  fn_801FD580(u8 *o);
    extern u8  fn_801FD568(u8 *o);
    extern u8  fn_801FD550(u8 *o);
    extern u8  fn_801FD538(u8 *o);
    extern u8  fn_801FD520(u8 *o);
    extern u8  fn_801FD508(u8 *o);
    extern u8  fn_801FD4F0(u8 *o);
    extern u8  fn_801FD4D8(u8 *o);
    extern u8  fn_801FD4C0(u8 *o);
    extern u8  fn_801FD4A8(u8 *o);
    extern u8  fn_801FD490(u8 *o);
    extern u8  fn_801FD478(u8 *o);
    extern u8  fn_801FD460(u8 *o);
    extern u8  fn_801FD448(u8 *o);
    extern u8  fn_801FD430(u8 *o);
    extern u8  fn_801FD418(u8 *o);
    extern u8  fn_801FD400(u8 *o);
    extern u8  fn_801FD3E8(u8 *o);
    extern u8  fn_801FD3D0(u8 *o);
    extern u8  fn_801FD3B8(u8 *o);
    extern u32 fn_801FD3A0(u8 *o);
    extern s16 fn_801FD388(u8 *o);
    extern u16 fn_801FD370(u8 *o);
    extern s16 fn_801FD358(u8 *o);
    extern u16 fn_801FD340(u8 *o);
    extern u8  fn_801FD188(u8 *o);
    extern s16 fn_801FD160(u8 *o);
    extern u32 fn_801FD11C(u8 *o, u32 i);
    extern u8  fn_802062FC(u8 *o);
    /* compound-case helpers */
    extern u32 fn_8011CE44(u32 mapId, u8 idx);
    extern u32 fn_8011CE74(u32 mapId);
    extern u32 fn_80131574(s32 idx);

    /* --- entry guards (selector range 1..0x123) --- */
    if (selector == 0 || selector >= 0x124) {
        return 0;
    }
    /* Selectors below 0x6D are "live" field properties: ping the refresh hook
     * with the sub-index, then require a non-NULL object.  (0x6D itself is a
     * default slot and never reaches a handler.) */
    if (selector < 0x6D) {
        fn_8011E778(id);
        if (obj == NULL) {
            return 0;
        }
    }

    switch (selector) {
    case 0x001: return fn_8011E760(obj);
    case 0x003: return fn_8011E734(obj);
    case 0x004: return fn_8011E708(obj);
    case 0x005: return fn_8011E6DC(obj);
    case 0x006: return fn_8011E6B0(obj);
    case 0x007: return fn_8011E684(obj);
    case 0x008: return fn_8011E658(obj);
    case 0x00A: return fn_8011E62C(obj);
    case 0x00B: return fn_8011E600(obj);
    case 0x00C: return fn_8011E5D4(obj);
    case 0x00D: return fn_8011E5A8(obj);
    case 0x00E: return fn_8011E57C(obj);
    case 0x00F: return fn_8011E550(obj);
    case 0x010: return fn_8011E538(obj);
    case 0x011: return fn_8011E520(obj);
    case 0x012: return fn_8011E508(obj);
    case 0x013: return fn_8011E4F0(obj);
    case 0x014: return fn_8011E4D8(obj);
    case 0x015: return fn_8011E4A4(obj, id);
    case 0x016: return fn_8011E474(obj, id);
    case 0x017: return fn_8011E444(obj, id);
    case 0x019: return fn_8011E3FC(obj, id);
    case 0x01A: return fn_8011E3B4(obj, id);
    case 0x01B: return fn_8011E36C(obj, id);
    case 0x01D: return fn_8011E324(obj, id);
    case 0x01E: return fn_8011E2DC(obj, id);
    /* 0x20..0x59: indexed sub-property getter, sub-index = selector - 0x20,
     * but the table skips index 0x19 between selectors 0x38 and 0x39. */
    case 0x020: return fn_8011E2AC(obj, 0x00);
    case 0x021: return fn_8011E2AC(obj, 0x01);
    case 0x022: return fn_8011E2AC(obj, 0x02);
    case 0x023: return fn_8011E2AC(obj, 0x03);
    case 0x024: return fn_8011E2AC(obj, 0x04);
    case 0x025: return fn_8011E2AC(obj, 0x05);
    case 0x026: return fn_8011E2AC(obj, 0x06);
    case 0x027: return fn_8011E2AC(obj, 0x07);
    case 0x028: return fn_8011E2AC(obj, 0x08);
    case 0x029: return fn_8011E2AC(obj, 0x09);
    case 0x02A: return fn_8011E2AC(obj, 0x0A);
    case 0x02B: return fn_8011E2AC(obj, 0x0B);
    case 0x02C: return fn_8011E2AC(obj, 0x0C);
    case 0x02D: return fn_8011E2AC(obj, 0x0D);
    case 0x02E: return fn_8011E2AC(obj, 0x0E);
    case 0x02F: return fn_8011E2AC(obj, 0x0F);
    case 0x030: return fn_8011E2AC(obj, 0x10);
    case 0x031: return fn_8011E2AC(obj, 0x11);
    case 0x032: return fn_8011E2AC(obj, 0x12);
    case 0x033: return fn_8011E2AC(obj, 0x13);
    case 0x034: return fn_8011E2AC(obj, 0x14);
    case 0x035: return fn_8011E2AC(obj, 0x15);
    case 0x036: return fn_8011E2AC(obj, 0x16);
    case 0x037: return fn_8011E2AC(obj, 0x17);
    case 0x038: return fn_8011E2AC(obj, 0x18);
    case 0x039: return fn_8011E2AC(obj, 0x1A);
    case 0x03A: return fn_8011E2AC(obj, 0x1B);
    case 0x03B: return fn_8011E2AC(obj, 0x1C);
    case 0x03C: return fn_8011E2AC(obj, 0x1D);
    case 0x03D: return fn_8011E2AC(obj, 0x1E);
    case 0x03E: return fn_8011E2AC(obj, 0x1F);
    case 0x03F: return fn_8011E2AC(obj, 0x20);
    case 0x040: return fn_8011E2AC(obj, 0x21);
    case 0x041: return fn_8011E2AC(obj, 0x22);
    case 0x042: return fn_8011E2AC(obj, 0x23);
    case 0x043: return fn_8011E2AC(obj, 0x24);
    case 0x044: return fn_8011E2AC(obj, 0x25);
    case 0x045: return fn_8011E2AC(obj, 0x26);
    case 0x046: return fn_8011E2AC(obj, 0x27);
    case 0x047: return fn_8011E2AC(obj, 0x28);
    case 0x048: return fn_8011E2AC(obj, 0x29);
    case 0x049: return fn_8011E2AC(obj, 0x2A);
    case 0x04A: return fn_8011E2AC(obj, 0x2B);
    case 0x04B: return fn_8011E2AC(obj, 0x2C);
    case 0x04C: return fn_8011E2AC(obj, 0x2D);
    case 0x04D: return fn_8011E2AC(obj, 0x2E);
    case 0x04E: return fn_8011E2AC(obj, 0x2F);
    case 0x04F: return fn_8011E2AC(obj, 0x30);
    case 0x050: return fn_8011E2AC(obj, 0x31);
    case 0x051: return fn_8011E2AC(obj, 0x32);
    case 0x052: return fn_8011E2AC(obj, 0x33);
    case 0x053: return fn_8011E2AC(obj, 0x34);
    case 0x054: return fn_8011E2AC(obj, 0x35);
    case 0x055: return fn_8011E2AC(obj, 0x36);
    case 0x056: return fn_8011E2AC(obj, 0x37);
    case 0x057: return fn_8011E2AC(obj, 0x38);
    case 0x058: return fn_8011E2AC(obj, 0x39);
    case 0x059: return fn_8011E2AC(obj, 0x3A);
    case 0x05A: return fn_8011E264(obj, id);
    case 0x05B: return fn_8011E21C(obj, id);
    case 0x05C: return fn_8011E1D4(obj, id);
    case 0x05D: return fn_8011E1BC(obj);
    case 0x05E: return fn_8011E1A4(obj);
    case 0x05F: return fn_8011E18C(obj);
    case 0x060: return fn_8011E174(obj);
    case 0x061: return fn_8011E15C(obj);
    case 0x062: return fn_8011E128(obj);
    case 0x063: return fn_8011E0F4(obj);
    case 0x064: return fn_8011E0DC(obj);
    case 0x065: return fn_8011E0C4(obj);
    case 0x066: return fn_8011E0AC(obj);
    case 0x068: return fn_8011E078(obj, id);
    case 0x069: return fn_8011E048(obj, id);
    case 0x06A: return fn_8011E030(obj);
    case 0x06B: return fn_8011E018(obj);
    case 0x06C: return fn_8011E000(obj);
    case 0x06E: return fn_8011F5C8(obj);
    case 0x06F: return fn_8011F5B0(obj);
    case 0x070: return fn_8011F598(obj);
    case 0x071: return fn_8011F580(obj);
    case 0x072: return fn_8011F568(obj);
    case 0x073: return fn_8011F550(obj);
    case 0x074: return fn_8011F538(obj);
    case 0x075: return fn_8011F520(obj);
    case 0x076: return fn_8011F508(obj);
    case 0x077: return fn_8011F4F0(obj);
    case 0x078: return fn_8011F4D8(obj);
    case 0x079: return fn_8011F4C0(obj);
    case 0x07A: return fn_8011F4A8(obj);
    case 0x07B: {
        /* count-leading-zeros of the 16-bit species/flags field (selector 0x83
         * = recursive lookup); preserved verbatim for behavior fidelity. */
        u32 v = GSfield_TransitionStateMachine(obj, 0, 0x83, 0) & 0xFFFF;
        return __cntlzw(v);
    }
    case 0x07C: return fn_8011F474(obj, id);
    case 0x07D: return fn_8011F45C(obj);
    case 0x07F: return fn_8011F228(obj, id);
    case 0x080: return fn_8011F1F0(obj, id);
    case 0x081: return fn_8011F1B8(obj, id);
    case 0x082: return fn_8011F1A0(obj);
    case 0x083: return fn_8011F188(obj);
    case 0x087: return fn_8011F15C(obj);
    case 0x088: return fn_8011F130(obj);
    case 0x089: return fn_8011F104(obj);
    case 0x08A: return fn_8011F0D8(obj);
    case 0x08B: return fn_8011F0AC(obj);
    case 0x08C: return fn_8011F080(obj);
    case 0x08D: return fn_8011F054(obj);
    case 0x08E: return fn_8011F028(obj);
    case 0x08F: return fn_8011EFFC(obj);
    case 0x090: return fn_8011EFD0(obj);
    case 0x091: return fn_8011EFA4(obj);
    case 0x092: return fn_8011EF78(obj);
    case 0x093: return fn_8011EF4C(obj);
    case 0x094: return fn_8011EF20(obj);
    case 0x095: return fn_8011EEF4(obj);
    case 0x096: return fn_8011EEC8(obj);
    case 0x097: return fn_8011EE9C(obj);
    case 0x098: return fn_8011EE70(obj);
    case 0x099: return fn_8011EE58(obj);
    case 0x09C: return fn_8011ECEC(obj);
    case 0x09D: return fn_8011ECC0(obj);
    case 0x09E: return fn_8011EC94(obj);
    case 0x09F: return fn_8011EC68(obj);
    case 0x0A0: return fn_8011EC3C(obj);
    case 0x0A1: return fn_8011EB48(obj);
    case 0x0A3: return fn_8011EB1C(obj);
    case 0x0A4: return fn_8011EAF0(obj);
    case 0x0A5: return fn_8011EAC4(obj);
    case 0x0A6: return fn_8011EA98(obj);
    case 0x0A7: return fn_8011EA6C(obj);
    case 0x0A8: return fn_8011EA40(obj);
    case 0x0A9: return fn_8011EA14(obj);
    case 0x0AA: return fn_8011E9E8(obj);
    case 0x0AB: return fn_8011E9BC(obj);
    case 0x0AC: return fn_8011E990(obj);
    case 0x0AD: return fn_8011E964(obj);
    case 0x0AE: return fn_8011E938(obj);
    case 0x0AF: return fn_8011E90C(obj);
    case 0x0B0: return fn_8011EC10(obj);
    case 0x0B1: return fn_8011EBE4(obj);
    case 0x0B2: return fn_8011EBB8(obj);
    case 0x0B3: return fn_8011EB8C(obj);
    case 0x0B4: return fn_8011EB60(obj);
    case 0x0B5: return fn_8011E8F4(obj);
    case 0x0B6: return fn_8011E8DC(obj);
    case 0x0B7: return fn_8011E868(obj);
    case 0x0B8: return fn_8011E850(obj);
    case 0x0B9: return fn_8011E838(obj);
    case 0x0BA: {
        /* Compare the Pokemon's region/floor (field 0x6F) against the floor
         * boundary derived from its area (field 0x6E -> field 0x13) and the
         * three GSfield zone bases (fn_80131574): return 1 if it sits past the
         * boundary, else 0; 2 when no object. */
        u32 floorVal, areaId, zoneIdx;
        s32 boundary;
        if (obj == NULL) {
            return 2;
        }
        floorVal = GSfield_TransitionStateMachine(obj, 0, 0x6F, 0) & 0xFFFF;
        areaId   = GSfield_TransitionStateMachine(obj, 0, 0x6E, 0) & 0xFFFF;
        zoneIdx  = GSfield_TransitionStateMachine((u8 *)0, areaId, 0x13, 0) & 0xFFFF;

        if ((s32)zoneIdx == (s32)(fn_80131574(0) & 0xFF)) {
            boundary = 0;
        } else if ((s32)zoneIdx == (s32)(fn_80131574(1) & 0xFF)) {
            boundary = 1;
        } else if ((s32)zoneIdx == (s32)(fn_80131574(2) & 0xFF)) {
            boundary = 2;
        } else {
            boundary = -1;
        }

        if ((s32)zoneIdx < (s32)(s8)boundary) {
            if (floorVal > (u32)(floorVal & 0xFF)) {
                /* unreachable as written in the original; kept for fidelity */
            }
            return (floorVal > (floorVal & 0xFF)) ? 1 : 0;
        }
        return 0;
    }
    case 0x0BB: return fn_8011E820(obj);
    case 0x0BC: return fn_8011E808(obj);
    case 0x0BD: return fn_8011E7D8(obj);
    case 0x0BE: return fn_8011E7F0(obj);
    case 0x0BF: {
        /* (field 0x6F) modulo 25 -- decimal-day style wrap. The original used a
         * reciprocal-multiply (0x51EC51EC) divide-by-25; expressed directly. */
        u32 v = GSfield_TransitionStateMachine(obj, 0, 0x6F, 0);
        return (u32)(v % 0x19) & 0xFF;
    }
    case 0x0C0: {
        /* Scan map-link indices (fn_8011CE44) for the first whose value exceeds
         * the Pokemon's stored count (field 0x79), within the area's link total
         * (fn_8011CE74 of field 0x6E -> field 0x11).  Returns the matching slot
         * index (1..0x64) or 0. */
        u32 areaId  = GSfield_TransitionStateMachine(obj, 0, 0x6E, 0) & 0xFFFF;
        u32 mapId   = GSfield_TransitionStateMachine((u8 *)0, areaId, 0x11, 0) & 0xFF;
        u32 limit   = GSfield_TransitionStateMachine(obj, 0, 0x79, 0);
        u32 linkCnt = fn_8011CE74(mapId);
        u32 i;
        if (mapId == linkCnt) {
            return 0;
        }
        for (i = 1; (s32)i < 0x65; i++) {
            if (fn_8011CE44(mapId, (u8)i) > limit) {
                return (i - 1) & 0xFF;
            }
        }
        return (i - 1) & 0xFF;
    }
    case 0x0C1: {
        /* 16-bit fingerprint test: XOR the high/low halves of fields 0x75 and
         * 0x6F together (plus a constant 8) and report whether the result is
         * non-zero.  Preserved as the original cntlzw materialisation. */
        u32 a, b, acc;
        if (obj == NULL) {
            return 0;
        }
        a = GSfield_TransitionStateMachine(obj, 0, 0x75, 0);
        b = GSfield_TransitionStateMachine(obj, 0, 0x6F, 0);
        acc = (a >> 16) ^ (a & 0xFFFF);
        acc = (b >> 16) ^ acc;
        acc = (b & 0xFFFF) ^ acc;
        acc = 8u ^ acc;
        return ((8u << __cntlzw(acc)) >> 31) & 0xFF;
    }
    case 0x0C2: return fn_8011ED68(obj);
    case 0x0C3: return fn_8011EE40(obj);
    case 0x0C4: return fn_8011ED18(obj);
    case 0x0C5: return fn_8011EE28(obj);
    case 0x0C6: return fn_8011EE10(obj);
    case 0x0C7: return fn_8011EDF8(obj);
    case 0x0C8: return fn_8011EDC4(obj, id);
    case 0x0C9: return fn_8011E7C0(obj);
    case 0x0CB: return fn_801FDB60(obj);
    case 0x0CC: return fn_801FDB48(obj);
    case 0x0CD: return fn_801FDB14(obj, id);
    case 0x0CE: return (u32)(s32)fn_801FDAFC(obj);
    case 0x0CF: return fn_801FDAE4(obj);
    case 0x0D0: return fn_801FDACC(obj);
    case 0x0D1: return fn_801FDAB4(obj);
    case 0x0D2: return fn_801FD8E0(obj);
    case 0x0D5: return fn_801FDA9C(obj);
    case 0x0D6: return fn_801FDA84(obj);
    case 0x0D7: return fn_801FDA6C(obj);
    case 0x0D8: return fn_801FDA38(obj, id);
    case 0x0D9: return fn_801FDA20(obj);
    case 0x0DA: return fn_80205184(obj);
    case 0x0DB: return fn_80205224(obj);
    case 0x0DC: {
        /* Indirect through the field-record handle (field 0xD9), then read
         * field-state 0x2C / 0x2B / 0x2D / 0x2F / 0x29 / 0x2E. */
        void *rec = (void *)GSfield_TransitionStateMachine(obj, 0, 0xD9, 0);
        return fn_8011BEB4(rec, 0, 0x2C, 0);
    }
    case 0x0DD: {
        void *rec = (void *)GSfield_TransitionStateMachine(obj, 0, 0xD9, 0);
        return fn_8011BEB4(rec, 0, 0x2B, 0);
    }
    case 0x0DE: {
        void *rec = (void *)GSfield_TransitionStateMachine(obj, 0, 0xD9, 0);
        return fn_8020990C(rec, id) & 0xFF;
    }
    case 0x0E0: {
        void *rec = (void *)GSfield_TransitionStateMachine(obj, 0, 0xD9, 0);
        return fn_802096E8(rec) & 0xFF;
    }
    case 0x0E1: {
        void *rec = (void *)GSfield_TransitionStateMachine(obj, 0, 0xD9, 0);
        return fn_8011BEB4(rec, 0, 0x2D, 0);
    }
    case 0x0E2: {
        void *rec = (void *)GSfield_TransitionStateMachine(obj, 0, 0xD9, 0);
        return fn_8011BEB4(rec, 0, 0x2F, 0);
    }
    case 0x0E3: {
        void *rec = (void *)GSfield_TransitionStateMachine(obj, 0, 0xD9, 0);
        return fn_8011BEB4(rec, 0, 0x29, 0);
    }
    case 0x0E4: {
        void *rec = (void *)GSfield_TransitionStateMachine(obj, 0, 0xD9, 0);
        return fn_8011BEB4(rec, 0, 0x2E, 0);
    }
    case 0x0E5: return fn_801FDA08(obj);
    case 0x0E6: return fn_801FD9F0(obj);
    case 0x0E7: return fn_801FD9D8(obj);
    case 0x0E8: return fn_801FD9C0(obj);
    case 0x0E9: return fn_801FD9A8(obj);
    case 0x0EA: return fn_801FD990(obj);
    case 0x0EB: return fn_801FD978(obj);
    case 0x0EC: return fn_801FD960(obj);
    case 0x0ED: return fn_801FD948(obj);
    case 0x0EE: return fn_801FD808(obj);
    case 0x0EF: return fn_801FD064(obj);
    case 0x0F0: return fn_801FD04C(obj);
    case 0x0F1: return fn_801FD034(obj);
    case 0x0F2: return fn_801FD01C(obj);
    case 0x0F3: return fn_801FD004(obj);
    case 0x0F4: return fn_801FCFEC(obj);
    case 0x0F5: return (u32)(s32)fn_801FCFD4(obj);
    case 0x0F6: return fn_801FCFBC(obj);
    case 0x0F7: return fn_801FCFA4(obj);
    case 0x0F8: return fn_801FCF8C(obj);
    case 0x0F9: return fn_801FD7E0(obj);
    case 0x0FA: return fn_801FD7C8(obj);
    case 0x0FB: return fn_801FD7B0(obj);
    case 0x0FC: return fn_801FD798(obj);
    case 0x0FD: return (u32)(s32)fn_801FD684(obj, (u8)id);
    case 0x0FE: return fn_801FD648(obj);
    case 0x0FF: return fn_801FD614(obj, (u8)id);
    case 0x100: return fn_801FD5D8(obj);
    case 0x101: return fn_801FD5B0(obj);
    case 0x102: return (u32)(s32)fn_801FD780(obj);
    case 0x103: return fn_801FD768(obj);
    case 0x104: return (u32)(s32)fn_801FD750(obj);
    case 0x105: return fn_801FD738(obj);
    case 0x106: return fn_801FD598(obj);
    case 0x107: return fn_801FD580(obj);
    case 0x108: return fn_801FD568(obj);
    case 0x109: return fn_801FD550(obj);
    case 0x10A: return fn_801FD538(obj);
    case 0x10B: return fn_801FD520(obj);
    case 0x10C: return fn_801FD508(obj);
    case 0x10D: return fn_801FD4F0(obj);
    case 0x10E: return fn_801FD4D8(obj);
    case 0x10F: return fn_801FD4C0(obj);
    case 0x110: return fn_801FD4A8(obj);
    case 0x111: return fn_801FD490(obj);
    case 0x112: return fn_801FD478(obj);
    case 0x113: return fn_801FD460(obj);
    case 0x114: return fn_801FD448(obj);
    case 0x115: return fn_801FD430(obj);
    case 0x116: return fn_801FD418(obj);
    case 0x117: return fn_801FD400(obj);
    case 0x118: return fn_801FD3E8(obj);
    case 0x119: return fn_801FD3D0(obj);
    case 0x11A: return fn_801FD3B8(obj);
    case 0x11B: return fn_801FD3A0(obj);
    case 0x11C: return (u32)(s32)fn_801FD388(obj);
    case 0x11D: return fn_801FD370(obj);
    case 0x11E: return (u32)(s32)fn_801FD358(obj);
    case 0x11F: return fn_801FD340(obj);
    case 0x120: return fn_801FD188(obj);
    case 0x121: return (u32)(s32)fn_801FD160(obj);
    case 0x122: return fn_801FD11C(obj, id);
    case 0x123: return fn_802062FC(obj);
    default:
        return 0;
    }
}

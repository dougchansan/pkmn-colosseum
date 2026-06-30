/* fn_80130A88 - Initialize a Pokemon interaction sequence for slot 0xC4/0x19.
 * Retrieves an interaction value from arg1 (selector 2, idx 0), builds a
 * dialog/menu descriptor in a local buffer with four string-id entries, then
 * dispatches the constructed descriptor via fn_80129F20.
 * arg2 is unused (dead parameter, kept for ABI compatibility with call sites). */
void fn_80130A88(u8* arg1, u32 arg2) {
    extern u32  fn_8012A5B0(u8* ptr, u32 selector, u32 idx);
    extern u32  gamedataGetStatus(u32 a, u32 b);
    extern void fn_801240C4(u8* a, u32 b, u32 c, u32 d);
    extern void fn_80123D58(u8* a, u32 b, u32 c);
    extern u32  fn_80124410(u8* a, u32 b, s32 c, u32 d, u32 e);
    extern void fn_801254B4(u8* a, u32 b, u32 c, u32 d, u32 e);
    extern void fn_8012546C(u8* a);
    extern void fn_80129F20(u8* ctx, u8* buf, u32 c, u32 d, u32 e);

    u8  buf[0x140];
    u32 val;
    u32 tmp;

    val = fn_8012A5B0(arg1, 2, 0);
    fn_801240C4(buf, 0xc4, 0x19, gamedataGetStatus(0, 1));
    fn_80123D58(buf, 0, 0x5d);
    fn_80123D58(buf, 1, 0xd8);
    fn_80123D58(buf, 2, 0x73);
    fn_80123D58(buf, 3, 0x10e);
    tmp = fn_80124410(buf, 0, -1, 0, val);
    fn_801254B4(buf, 0, 0x6f, 0, tmp);
    fn_801254B4(buf, 0, 0x79, 0, 0x3ff4);
    fn_801254B4(buf, 0, 0x99, 0, 0xdc);
    fn_8012546C(buf);
    fn_80129F20(arg1, buf, 0xfe, 4, 0);
}

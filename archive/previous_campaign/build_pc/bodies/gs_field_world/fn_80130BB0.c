/* Build a dialog/menu buffer for the Pokemon-object 'obj', populate it with
 * four string entries, query a slot, set three properties, finalize, and
 * dispatch it via fn_80129F20. */
void fn_80130BB0(u8* obj)
{
    extern u32  fn_8012A5B0(u8* ptr, u32 selector, u32 idx);
    extern u32  gamedataGetStatus(u32 a, u32 b);
    extern void fn_801240C4(u8* buf, u32 a, u32 b, u32 c);
    extern void fn_80123D58(u8* buf, u32 idx, u32 str_id);
    extern u32  fn_80124410(u8* buf, u32 a, s32 b, u32 c, u32 val);
    extern void fn_801254B4(void* obj, u32 p, u16 sel, u32 a6, u32 a7);
    extern void fn_8012546C(u8* buf);
    extern void fn_80129F20(u32 ctx, u8* buf, u32 a, u32 b, u32 c);

    u8  buf[0x140];
    u32 interaction;
    u32 slot;
    u32 gamedata_status;

    /* Retrieve the interaction index for slot 2 of this object */
    interaction = fn_8012A5B0(obj, 2, 0);

    /* Initialize the buffer: type 0xc5, subtype 0x1a, with current game status */
    gamedata_status = gamedataGetStatus(0, 1);
    fn_801240C4(buf, 0xc5, 0x1a, gamedata_status);

    /* Add four string entries */
    fn_80123D58(buf, 0, 0x2c);
    fn_80123D58(buf, 1, 0x122);
    fn_80123D58(buf, 2, 0x10d);
    fn_80123D58(buf, 3, 0x121);

    /* Query a slot using the interaction value */
    slot = fn_80124410(buf, 0, -1, 0, interaction);

    /* Set three properties on the object buffer */
    fn_801254B4(buf, 0, (u16)0x6f, 0, slot);
    fn_801254B4(buf, 0, (u16)0x79, 0, 0x4a6f);
    fn_801254B4(buf, 0, (u16)0x99, 0, 0xdc);

    /* Finalize the buffer */
    fn_8012546C(buf);

    /* Dispatch / show the menu attached to 'obj' */
    fn_80129F20((u32)obj, buf, 0xfe, 4, 0);
}

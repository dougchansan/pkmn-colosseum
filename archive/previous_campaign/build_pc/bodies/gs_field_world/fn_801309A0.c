/* NPC battle trigger setup - single-encounter variant (species class 0x161, arena 0x0d).
 * Allocates a battle-parameter buffer on the stack, sets party/arena/difficulty fields,
 * adds participant slot 0xfe with item 0x911D, finalises the buffer, commits it to the
 * memo data store, then hands off to the field-world event dispatcher. */
void fn_801309A0(u8* arg1)
{
    extern u32  gamedataGetStatus(u32 a, u32 b);
    extern void fn_801240C4(u8* buf, u32 field_id, u32 npc_count, u32 data_val);
    extern void fn_801254B4(u8* buf, u32 p1, u16 sel, u32 a6, u32 v);
    extern u32  fn_800FA280(u32 val);
    extern void fn_80123EF0(u8* buf, u32 p1, u32 p2, u32 p3, u32 p4, u32 tag, u32 extra);
    extern u32  fn_80124410(u8* buf, s32 p1, s32 p2, u32 p3, u32 tag);
    extern void fn_8012546C(u8* buf);
    extern void fn_8025FF9C(u32 slot, u8* buf);   /* memoDataSet */
    extern s32  fn_80129E20(u8* ctx, u8* buf, u32 flag);

    u8  local[0x144];
    u32 tmp;

    /* Initialise battle buffer: species class 0x161, arena slot 0x0d,
     * trainer ID sourced from game-data status(0,1). */
    fn_801240C4(local, 0x161, 0x0d, gamedataGetStatus(0, 1));

    /* Set difficulty/party flags: selector 0x99 = 0x46 */
    fn_801254B4(local, 0, 0x99, 0, 0x46);

    /* Resolve item token 0x12ac into the encounter tag register */
    tmp = fn_800FA280(0x12ac);

    /* Configure participant slot 0xfe: arena 0x0d, 4 Pokémon, no auto-level,
     * tag 0x911D (= 0x10000 - 0x6EE3), item from fn_800FA280 above */
    fn_80123EF0(local, 0xfe, 0x0d, 4, 0, 0x911D, tmp);

    /* Query the resulting level cap for slot 0xfe / tag 0x911D */
    tmp = fn_80124410(local, -1, -1, 0, 0x911D);

    /* Store level cap into selector 0x6f */
    fn_801254B4(local, 0, 0x6f, 0, tmp);

    /* Finalise buffer (checksum / fixup) */
    fn_8012546C(local);

    /* Commit to memo data store, slot 0 */
    fn_8025FF9C(0, local);

    /* Hand off to field-world event dispatcher */
    fn_80129E20(arg1, local, 1);
}

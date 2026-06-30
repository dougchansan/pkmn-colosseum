/*
 * fn_80130890 - Create and register a memo/attestation for a field object.
 * Reads gender (selector 4) and level (selector 5) from game data, builds
 * an attestation record into a stack buffer, populates its properties, then
 * commits it via memoDataSet and links it to the field object.
 */
void fn_80130890(u8* field_obj)
{
    extern u32  gamedataGetStatus(u32 a, u32 b);
    extern void gamedataAttestCreate(u32* out, u32 type, u32 count, u8 gender, u8 level);
    extern void fn_801240C4(u8* buf, u32 a, u32 b, u32* src);
    extern void fn_801254B4(u8* buf, u32 a, u32 b, u32 c, u32 d);
    extern u32  fn_800FA280(u32 val);
    extern void fn_80123EF0(u8* buf, u32 a, u32 b, u32 c, u32 d, u32 e, u32 f);
    extern u32  fn_80124410(u8* buf, s32 a, s32 b, u32 c, u32 d);
    extern void fn_8012546C(u8* buf);
    extern void memoDataSet(u32 slot, u8* buf);
    extern s32  fn_80129E20(u8* obj, u8* buf, u32 flags);

    u8  level;
    u8  gender;
    u32 attest_out;
    u8  buf[0x13c];
    u32 query_result;

    /* gamedataGetStatus(0, 5) -> level (u8), gamedataGetStatus(0, 4) -> gender (u8) */
    level  = (u8)gamedataGetStatus(0, 5);
    gender = (u8)gamedataGetStatus(0, 4);

    /* Build attestation header; type=8, count=3, gender and level from above */
    gamedataAttestCreate(&attest_out, 0x8, 3, gender, level);

    /* Initialize record buffer from the attestation + selector 0xfa/0x46 */
    fn_801240C4(buf, 0xfa, 0x46, &attest_out);

    /* Set property selector 0x99 = 0x46 on buf */
    fn_801254B4(buf, 0, 0x99, 0, 0x46);

    /* Resolve a lookup value then populate property selectors 0xff/0x46 */
    fn_80123EF0(buf, 0xff, 0x46, 4, 0, 0x2740, fn_800FA280(0x12ad));

    /* Query and forward result to selector 0x6f */
    query_result = fn_80124410(buf, -1, -1, 0, 0x2740);
    fn_801254B4(buf, 0, 0x6f, 0, query_result);

    /* Finalise buffer, commit to memo slot 0, link to field object */
    fn_8012546C(buf);
    memoDataSet(0, buf);
    fn_80129E20(field_obj, buf, 1);
}

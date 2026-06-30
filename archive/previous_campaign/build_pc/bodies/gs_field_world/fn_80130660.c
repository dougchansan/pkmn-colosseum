/* fn_80130660 - Build a Pokemon "attest" record (disc-check variant 0xFB) and attach it to arg1.
 * Creates a witness record using the current game's gender/species status, populates a
 * stack-local attest buffer, sets properties, resolves a name string, finalises the record,
 * registers it with the memo store, then links it into arg1's chain.
 * Sibling of fn_80130770 (0x19 variant); differs in fn_801240C4 type arg (0xfb) and
 * fn_80123EF0 flag arg5 (1 vs 0). */
void fn_80130660(u8* arg1) {
    extern u32  gamedataGetStatus(u32 a, u32 b);
    extern void gamedataAttestCreate(u32* a, u32 b, u32 c, u8 d, u8 e);
    extern void fn_801240C4(u8* a, u32 b, u32 c, u32* d);
    extern void fn_801254B4(u8* a, u32 b, u32 c, u32 d, u32 e);
    extern u32  fn_800FA280(u32 val);
    extern void fn_80123EF0(u8* a, u32 b, u32 c, u32 d, u32 e, u32 f, u32 g);
    extern u32  fn_80124410(u8* a, s32 b, s32 c, u32 d, u32 e);
    extern void fn_8012546C(u8* a);
    extern void memoDataSet(u32 a, u8* b);
    extern s32  fn_80129E20(u8* a, u8* b, u32 c);

    u8  gender  = (u8)gamedataGetStatus(0, 5);
    u8  species = (u8)gamedataGetStatus(0, 4);

    u32 attest_id;
    gamedataAttestCreate(&attest_id, 0x9, 3, species, gender);

    u8 buf[0x13c];
    fn_801240C4(buf, 0xfb, 0xa, &attest_id);
    fn_801254B4(buf, 0, 0x99, 0, 0x46);

    u32 name_str = fn_800FA280(0x12af);
    fn_80123EF0(buf, 0xff, 0xa, 4, 1, 0x7991, name_str);

    u32 resolved = fn_80124410(buf, -1, -1, 0, 0x7991);
    fn_801254B4(buf, 0, 0x6f, 0, resolved);

    fn_8012546C(buf);
    memoDataSet(0, buf);
    fn_80129E20(arg1, buf, 1);
}

/*
 * fn_80130770 (0x80130770, size 0x120)
 * Builds an "attest" (Pokemon witness/memo) record for party slot 0x19,
 * stamps it with game-data gender/level, configures experience/stat fields,
 * and attaches it to the caller's context object.
 * Sibling of fn_80130660 (slot 0xfb) and fn_80130890 (slot 0x20).
 */
void fn_80130770(u8* obj)
{
    extern u32  gamedataGetStatus(u32 a, u32 b);
    extern void gamedataAttestCreate(u32* a, u32 b, u32 c, u8 d, u8 e);
    extern void fn_801240C4(u8* a, u32 b, u32 c, u32* d);
    extern void fn_801254B4(u8* a, u32 b, u32 c, u32 d, u32 e);
    extern u32  fn_800FA280(u32 val);
    extern void fn_80123EF0(u8* a, u32 b, u32 c, u32 d, u32 e, u32 f, u32 g);
    extern u32  fn_80124410(u8* a, s32 b, s32 c, u32 d, u32 e);
    extern u16  fn_80123110(u8* a, u32 b, u8 c);
    extern void fn_8012546C(u8* a);
    extern void memoDataSet(u32 a, u8* b);
    extern s32  fn_80129E20(u8* a, u8* b, u32 c);

    u8  gender;
    u8  level;
    u32 attest;
    u8  buf[0x13c];
    u32 exp;

    gender = (u8)gamedataGetStatus(0, 5);
    level  = (u8)gamedataGetStatus(0, 4);
    gamedataAttestCreate(&attest, 0x9, 3, level, gender);
    fn_801240C4(buf, 0x19, 0xa, &attest);
    fn_801254B4(buf, 0, 0x99, 0, 0x46);
    fn_80123EF0(buf, 0xff, 0xa, 4, 0, 0x7991, fn_800FA280(0x12ae));
    exp = fn_80124410(buf, -1, -1, 0, 0x7991);
    fn_801254B4(buf, 0, 0x6f, 0, exp);
    fn_80123110(buf, 0xca, 1);
    fn_8012546C(buf);
    memoDataSet(0, buf);
    fn_80129E20(obj, buf, 1);
}

#include "dolphin/types.h"

extern u8 HSD_AObjGetAllocData[];
extern u8 HSD_ChanGetAllocData[];
extern u8 HSD_DListGetAllocData[];
extern u8 HSD_FObjGetAllocData[];
extern u8 HSD_IDGetAllocData[];
extern u8 HSD_MtxGetAllocData[];
extern u8 HSD_RObjGetAllocData[];
extern u8 HSD_RenderGetAllocData[];
extern u8 HSD_RvalueObjGetAllocData[];
extern u8 HSD_SListGetAllocData[];
extern u8 HSD_ShadowGetAllocData[];
extern u8 HSD_TevRegGetAllocData[];
extern u8 HSD_VecGetAllocData[];
extern u8 WS_ABARERU[];
extern u8 WS_AKUBI[];
extern u8 WS_ALERTEND[];
extern u8 WS_CHECK_TYPE[];
extern u8 WS_CHIISAKUNARU[];
extern u8 WS_CHOUHATSU[];
extern u8 WS_CONDITION_CHECK[];
extern u8 WS_CRITICAL_CHECK[];
extern u8 WS_DAMAGE_LOSS[];
extern u8 WS_DAMAGE_LOSS_ONLY[];
extern u8 WS_GETEND[];
extern u8 WS_HIMITUNOTIKARA[];
extern u8 WS_HITCHECK[];
extern u8 WS_ICHAMON[];
extern u8 WS_ITEMEND[];
extern u8 WS_JIKOANJI[];
extern u8 WS_JUUDEN[];
extern u8 WS_KIERUTAME[];
extern u8 WS_KIERUTAME_AFTAR[];
extern u8 WS_KORAERU_CHECK[];
extern u8 WS_KUROIKIRI[];
extern u8 WS_MARUKUNARU[];
extern u8 WS_MICHIDURE[];
extern u8 WS_MIYABURU[];
extern u8 WS_NARIKIRI[];
extern u8 WS_NEKODAMASHI[];
extern u8 WS_NEWOHARU[];
extern u8 WS_ONNEN[];
extern u8 WS_SCA_END_SET[];
extern u8 WS_SEQEND[];
extern u8 WS_SEQRET[];
extern u8 WS_SIDECONDITION_CHECK[];
extern u8 WS_SPEABIEND[];
extern u8 WS_STATUS_GET16[];
extern u8 WS_STATUS_GET32[];
extern u8 WS_STATUS_GET8[];
extern u8 WS_STATUS_SET16[];
extern u8 WS_STATUS_SET32[];
extern u8 WS_STATUS_SET8[];
extern u8 WS_SWITCH_A_D[];
extern u8 WS_TSUIKA_DIRECT_ACT[];
extern u8 WS_TSUIKA_INDIRECT_ACT[];
extern u8 WS_TYPE_CHECK[];
extern u8 WS_WAZAEND[];
extern u8 WS_WAZAKOUKA_CHECK[];
extern u8 WS_WAZAOBOE_CHECK[];
extern u8 WS_WEATHER_CHANGE[];
extern u8 _HSD_AObjForgetMemory[];
extern u8 _HSD_DispForgetMemory[];
extern u8 _HSD_IDForgetMemory[];
extern u8 _HSD_ObjAllocForgetMemory[];
extern u8 _HSD_RObjForgetMemory[];
extern u8 _HSD_RandForgetMemory[];
extern u8 _dbgMenuFightGetWazaDataIdSub[];
extern u8 fn_80005FFC[];
extern u8 dbgMenuFightStop[];
extern u8 dbgMenuPokemonEdit[];
extern u8 _floorGetStateSize[];
extern u8 _floorMakeStateData[];
extern u8 _floorRestoreStateData[];
extern u8 fn_80118068[];
extern u8 tvRelease2[];
extern u8 fn_80118100[];
extern u8 _cameraGetStateSize[];
extern u8 _cameraMakeStateData[];
extern u8 _cameraRestoreStateData[];
extern u8 wsCmdF5[];
extern u8 wsCmdF4[];
extern u8 wsCmdF3[];
extern u8 wsCmdF2[];
extern u8 wsCmdF1[];
extern u8 wsCmdF0[];
extern u8 wsCmdEF[];
extern u8 wsCmdEE[];
extern u8 wsCmdED[];
extern u8 wsCmdEC[];
extern u8 wsCmdEB[];
extern u8 wsCmdEA[];
extern u8 wsCmdE9[];
extern u8 wsCmdE8[];
extern u8 wsCmdE7[];
extern u8 wsCmdE6[];
extern u8 wsCmdE5[];
extern u8 wsCmdE3[];
extern u8 wsCmdE2[];
extern u8 wsCmdE1[];
extern u8 wsCmdE0[];
extern u8 wsCmdDF[];
extern u8 wsCmdDE[];
extern u8 wsCmdDD[];
extern u8 wsCmdDB[];
extern u8 wsCmdDA[];
extern u8 wsCmdD9[];
extern u8 wsCmdD8[];
extern u8 wsCmdD6[];
extern u8 wsCmdD4[];
extern u8 wsCmdD2[];
extern u8 wsCmdD1[];
extern u8 wsCmdCF[];
extern u8 wsCmdCD[];
extern u8 wsCmdCC[];
extern u8 wsCmdCA[];
extern u8 wsCmdC9[];
extern u8 wsCmdC8[];
extern u8 wsCmdC4[];
extern u8 wsCmdC3[];
extern u8 wsCmdC2[];
extern u8 wsCmdC1[];
extern u8 wsCmdC0[];
extern u8 wsCmdBE[];
extern u8 wsCmdBC[];
extern u8 wsCmdBB[];
extern u8 wsCmdBA[];
extern u8 wsCmdB9[];
extern u8 wsCmdB8[];
extern u8 wsCmdB7[];
extern u8 wsCmdB6[];
extern u8 wsCmdB5[];
extern u8 wsCmdB4[];
extern u8 wsCmdB3[];
extern u8 wsCmdB2[];
extern u8 wsCmdB0[];
extern u8 wsCmdAF[];
extern u8 wsCmdAE[];
extern u8 wsCmdAD[];
extern u8 wsCmdAC[];
extern u8 wsCmdAB[];
extern u8 wsCmdA9[];
extern u8 wsCmdA8[];
extern u8 wsCmdA7[];
extern u8 wsCmdA6[];
extern u8 wsCmdA5[];
extern u8 wsCmdA4[];
extern u8 wsCmdA3[];
extern u8 wsCmdA2[];
extern u8 wsCmdA1[];
extern u8 wsCmdA0[];
extern u8 wsCmd9F[];
extern u8 wsCmd9E[];
extern u8 wsCmd9D[];
extern u8 wsCmd9C[];
extern u8 wsCmd9B[];
extern u8 wsCmd9A[];
extern u8 wsCmd99[];
extern u8 wsCmd98[];
extern u8 wsCmd97[];
extern u8 wsCmd96[];
extern u8 wsCmd94[];
extern u8 fightCheckOneHitKO[];
extern u8 wsCmd92[];
extern u8 wsCmd91[];
extern u8 wsCmd90[];
extern u8 wsCmd8F[];
extern u8 wsCmd8E[];
extern u8 wsCmd8D[];
extern u8 wsCmd8B[];
extern u8 wsCmd89[];
extern u8 wsCmd88[];
extern u8 wsCmd87[];
extern u8 wsCmd86[];
extern u8 wsCmd85[];
extern u8 wsCmd84[];
extern u8 wsCmd83[];
extern u8 wsCmd81[];
extern u8 wsCmd80[];
extern u8 wsCmd7F[];
extern u8 wsCmd7E[];
extern u8 wsCmd7D[];
extern u8 wsCmd7C[];
extern u8 wsCmd7B[];
extern u8 wsCmd7A[];
extern u8 wsCmd79[];
extern u8 wsCmd78[];
extern u8 wsCmd77[];
extern u8 wsCmd76[];
extern u8 wsCmd75[];
extern u8 wsCmd74[];
extern u8 wsCmd73[];
extern u8 wsCmd72[];
extern u8 wsCmd70[];
extern u8 wsCmd6F[];
extern u8 wsCmd6E[];
extern u8 wsCmd6D[];
extern u8 wsCmd6C[];
extern u8 wsCmd6B[];
extern u8 wsCmd6A[];
extern u8 wsCmd67[];
extern u8 wsCmd66[];
extern u8 wsCmd65[];
extern u8 wsCmd64[];
extern u8 wsCmd63[];
extern u8 wsCmd62[];
extern u8 wsCmd61[];
extern u8 wsCmd60[];
extern u8 wsCmd5E[];
extern u8 wsCmd5D[];
extern u8 wsCmd5C[];
extern u8 wsCmd5B[];
extern u8 wsCmd5A[];
extern u8 wsCmd71[];
extern u8 wsCmd58[];
extern u8 wsCmd57[];
extern u8 wsCmd56[];
extern u8 wsCmd55[];
extern u8 wsCmd54[];
extern u8 wsCmd53[];
extern u8 wsCmd52[];
extern u8 wsCmd51[];
extern u8 wsCmd50[];
extern u8 wsCmd4F[];
extern u8 wsCmd4E[];
extern u8 wsCmd4D[];
extern u8 wsCmd4C[];
extern u8 wsCmd4B[];
extern u8 wsCmd49[];
extern u8 wsCmd48[];
extern u8 wsCmd47[];
extern u8 wsCmd46[];
extern u8 wsCmd45[];
extern u8 wsCmd43[];
extern u8 wsCmd42[];
extern u8 wsCmd41[];
extern u8 wsCmd3B[];
extern u8 wsCmd3A[];
extern u8 wsCmd39[];
extern u8 wsCmd38[];
extern u8 wsCmd37[];
extern u8 wsCmd36[];
extern u8 wsCmd35[];
extern u8 wsCmd34[];
extern u8 wsCmd33[];
extern u8 wsCmd32[];
extern u8 wsCmd31[];
extern u8 wsCmd30[];
extern u8 wsCmd2F[];
extern u8 wsCmd2E[];
extern u8 wsCmd2D[];
extern u8 wsCmd2C[];
extern u8 wsCmd2B[];
extern u8 wsCmd2A[];
extern u8 wsCmd29[];
extern u8 wsCmd28[];
extern u8 wsCmd27[];
extern u8 wsCmd26[];
extern u8 wsCmd25[];
extern u8 wsCmd24[];
extern u8 wsCmd23[];
extern u8 wsCmd20[];
extern u8 wsCmd1E[];
extern u8 wsCmd1D[];
extern u8 wsCmd1B[];
extern u8 wsCmd1A[];
extern u8 wsCmd19[];
extern u8 wsCmd18[];
extern u8 wsCmd15[];
extern u8 wsCmd14[];
extern u8 wsCmd13[];
extern u8 wsCmd12[];
extern u8 wsCmd11[];
extern u8 wsCmd10[];
extern u8 wsCmd0F[];
extern u8 wsCmd0E[];
extern u8 wsCmd0D[];
extern u8 wsCmd0C[];
extern u8 wsCmd0B[];
extern u8 wsCmd0A[];
extern u8 wsCmd09[];
extern u8 wsCmd4A[];
extern u8 wsCmd05[];
extern u8 wsCmd03[];
extern u8 wsCmd02[];
extern u8 fightCheckAccuracy[];
extern u8 wsCmd40[];
extern u8 fightGSfloorGetPushDataSize[];
extern u8 fightGSfloorPushData[];
extern u8 fightGSfloorPopData[];
extern u8 lbl_80004000[];
extern u8 lbl_8047DAB0[];
extern u8 lbl_8047DAB8[];
extern u8 lbl_8047DAC0[];
extern u8 lbl_8047DAC4[];
extern u8 lbl_8047DACC[];
extern u8 lbl_8047DAD4[];
extern u8 lbl_8047DAD8[];
extern u8 lbl_8047DADC[];
extern u8 lbl_8047DAE4[];
extern u8 lbl_8047DAEC[];
extern u8 lbl_8047DAF4[];
extern u8 lbl_8047DAFC[];
extern u8 lbl_8047DB04[];
extern u8 peopleBiosGetPushDataSize[];
extern u8 peopleBiosPopData[];
extern u8 peopleBiosPushData[];

/* Auto-carved .rodata unit 0x80275638..0x80279A68 (76 objects). Non-relocated data emitted byte-exact as const u8[]; pointer/jump tables as const void*[] for R_PPC_ADDR32 relocations. */

const u8 lbl_802757F0[12] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
const u8 lbl_802757FC[11] = {
    0x73, 0x63, 0x65, 0x6E, 0x65, 0x5F, 0x64, 0x61, 0x74, 0x61, 0x00,
};
const u8 gap_04_80275807_rodata[1] = {
    0x00,
};
const u8 lbl_80275808[40] = {
    0x62, 0x61, 0x74, 0x74, 0x6C, 0x65, 0x47, 0x72, 0x69, 0x64, 0x52, 0x65, 0x70, 0x6C, 0x61, 0x63,
    0x65, 0x50, 0x6F, 0x6B, 0x65, 0x6D, 0x6F, 0x6E, 0x3A, 0x20, 0x4E, 0x55, 0x4C, 0x4C, 0x20, 0x61,
    0x72, 0x67, 0x75, 0x6D, 0x65, 0x6E, 0x74, 0x00,
};
const u8 lbl_80275830[41] = {
    0x62, 0x61, 0x74, 0x74, 0x6C, 0x65, 0x47, 0x72, 0x69, 0x64, 0x52, 0x65, 0x70, 0x6C, 0x61, 0x63,
    0x65, 0x54, 0x72, 0x61, 0x69, 0x6E, 0x65, 0x72, 0x3A, 0x20, 0x4E, 0x55, 0x4C, 0x4C, 0x20, 0x61,
    0x72, 0x67, 0x75, 0x6D, 0x65, 0x6E, 0x74, 0x21, 0x00,
};
const u8 gap_04_80275859_rodata[7] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
const u8 lbl_80275860[59] = {
    0x57, 0x61, 0x72, 0x6E, 0x69, 0x6E, 0x67, 0x3A, 0x20, 0x66, 0x61, 0x64, 0x65, 0x46, 0x6C, 0x75,
    0x69, 0x64, 0x20, 0x63, 0x20, 0x70, 0x61, 0x72, 0x6D, 0x2E, 0x20, 0x4C, 0x69, 0x6D, 0x69, 0x74,
    0x20, 0x4F, 0x76, 0x65, 0x72, 0x21, 0x21, 0x20, 0x2D, 0x3E, 0x20, 0x30, 0x2E, 0x30, 0x66, 0x20,
    0x3C, 0x20, 0x25, 0x66, 0x20, 0x3C, 0x20, 0x25, 0x66, 0x0A, 0x00,
};
const u8 gap_04_8027589B_rodata[5] = {
    0x00, 0x00, 0x00, 0x00, 0x00,
};
const u8 lbl_802758A0[12] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
const u8 lbl_802758AC[28] = {
    0x83, 0x89, 0x83, 0x43, 0x83, 0x67, 0x5B, 0x25, 0x64, 0x5D, 0x82, 0xCC, 0x8E, 0xE6, 0x93, 0xBE,
    0x82, 0xC9, 0x82, 0xB5, 0x82, 0xC1, 0x82, 0xCF, 0x82, 0xA2, 0x0A, 0x00,
};
const u8 lbl_802758C8[32] = {
    0x5B, 0x25, 0x73, 0x5D, 0x20, 0x83, 0x58, 0x83, 0x8C, 0x83, 0x62, 0x83, 0x68, 0x82, 0xAA, 0x82,
    0xA0, 0x82, 0xE8, 0x82, 0xDC, 0x82, 0xB9, 0x82, 0xF1, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

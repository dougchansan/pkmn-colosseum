#include "dolphin/types.h"

#pragma section ".data"

extern u8 pad_05_80363774_data[];
extern void* msgctrlcode[];
extern u8 msgctrlAlign[];
extern u8 msgctrlAttackMons[];
extern u8 msgctrlBaseLineBias[];
extern u8 msgctrlCR[];
extern u8 msgctrlClientMos[];
extern u8 msgctrlClientnowork[];
extern u8 msgctrlColor[];
extern u8 msgctrlDeffenceMons[];
extern u8 msgctrlDigit[];
extern u8 msgctrlDigit2[];
extern u8 msgctrlEnemyMons[];
extern u8 msgctrlEnemyMons2[];
extern u8 msgctrlEnemyTmons[];
extern u8 msgctrlEnemyTmons2[];
extern u8 msgctrlEvStrBuf0[];
extern u8 msgctrlEvStrBuf1[];
extern u8 msgctrlEvStrBuf2[];
extern u8 msgctrlFont[];
extern u8 msgctrlHero[];
extern u8 msgctrlHizuki[];
extern u8 msgctrlIndentOff[];
extern u8 msgctrlItem[];
extern u8 msgctrlItem2[];
extern u8 msgctrlItemName[];
extern u8 msgctrlKeyEnd[];
extern u8 msgctrlKeyWait[];
extern u8 msgctrlLineSpace[];
extern u8 msgctrlMenuDigit[];
extern u8 msgctrlMenuDigit2[];
extern u8 msgctrlMenuFullDigit[];
extern u8 msgctrlMenuHex[];
extern u8 msgctrlMenuHex2[];
extern u8 msgctrlMenuMoney[];
extern u8 msgctrlMenuMsg[];
extern u8 msgctrlMenuMsg2[];
extern u8 msgctrlMenuMsgID[];
extern u8 msgctrlMenuMsgID2[];
extern u8 msgctrlMenuPokemon[];
extern u8 msgctrlMenuUDigit[];
extern u8 msgctrlMenuUDigit2[];
extern u8 msgctrlMenuZDigit[];
extern u8 msgctrlMenuZDigit2[];
extern u8 msgctrlMoney[];
extern u8 msgctrlMsgID[];
extern u8 msgctrlMyMons[];
extern u8 msgctrlMyMons2[];
extern u8 msgctrlMyName[];
extern u8 msgctrlNpc[];
extern u8 msgctrlPalette[];
extern u8 msgctrlPasoName[];
extern u8 msgctrlPokemon[];
extern u8 msgctrlPokemon2[];
extern u8 msgctrlPokemonID[];
extern u8 msgctrlRubyEnd[];
extern u8 msgctrlRubyStart[];
extern u8 msgctrlRubyTop[];
extern u8 msgctrlSeOff[];
extern u8 msgctrlSeOn[];
extern u8 msgctrlShadow[];
extern u8 msgctrlSideAttackNameha[];
extern u8 msgctrlSideAttackNameno[];
extern u8 msgctrlSideAttackNamewo[];
extern u8 msgctrlSideDefenceNameha[];
extern u8 msgctrlSideDefenceNameno[];
extern u8 msgctrlSideDefenceNamewo[];
extern u8 msgctrlSndPlay[];
extern u8 msgctrlSndWait[];
extern u8 msgctrlSpeabiNamea[];
extern u8 msgctrlSpeabiNamec[];
extern u8 msgctrlSpeabiNamed[];
extern u8 msgctrlSpeabiNamet[];
extern u8 msgctrlString[];
extern u8 msgctrlString2[];
extern u8 msgctrlTalkSE[];
extern u8 msgctrlTime[];
extern u8 msgctrlTrainerClientno[];
extern u8 msgctrlTrainerEnename[];
extern u8 msgctrlTrainerEnename2[];
extern u8 msgctrlTrainerLose[];
extern u8 msgctrlTrainerName[];
extern u8 msgctrlTrainerType[];
extern u8 msgctrlTribe[];
extern u8 msgctrlTsuikaMons[];
extern u8 msgctrlWait[];
extern u8 msgctrlWaza[];
extern u8 msgctrlWazaName[];

/* Prefix of the original data unit, ending before the gamedatasave switch tables. */

u8 pad_05_80363774_data[4] = {
    0x00, 0x00, 0x00, 0x00,
};

void* msgctrlcode[190] = {
    (void*)0x08000000,
    (void*)((u8*)msgctrlCR),
    (void*)0x08000000,
    (void*)((u8*)msgctrlCR),
    (void*)0x30000000,
    (void*)((u8*)msgctrlKeyEnd),
    (void*)0x38000000,
    (void*)((u8*)msgctrlKeyWait),
    (void*)0x18000000,
    (void*)((u8*)msgctrlRubyStart),
    (void*)0x18000000,
    (void*)((u8*)msgctrlRubyTop),
    (void*)0x18000000,
    (void*)((u8*)msgctrlRubyEnd),
    (void*)0x18000000,
    (void*)((u8*)msgctrlFont),
    (void*)0x18000000,
    (void*)((u8*)msgctrlColor),
    (void*)0x38000000,
    (void*)((u8*)msgctrlWait),
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x58000000,
    (void*)((u8*)msgctrlEvStrBuf0),
    (void*)0x58000000,
    (void*)((u8*)msgctrlEvStrBuf1),
    (void*)0x58000000,
    (void*)((u8*)msgctrlAttackMons),
    (void*)0x58000000,
    (void*)((u8*)msgctrlDeffenceMons),
    (void*)0x58000000,
    (void*)((u8*)msgctrlClientMos),
    (void*)0x58000000,
    (void*)((u8*)msgctrlTsuikaMons),
    (void*)0x58000000,
    (void*)((u8*)msgctrlMyName),
    (void*)0x58000000,
    (void*)((u8*)msgctrlMyMons),
    (void*)0x58000000,
    (void*)((u8*)msgctrlMyMons2),
    (void*)0x58000000,
    (void*)((u8*)msgctrlEnemyMons),
    (void*)0x58000000,
    (void*)((u8*)msgctrlEnemyMons2),
    (void*)0x58000000,
    (void*)((u8*)msgctrlEnemyTmons),
    (void*)0x58000000,
    (void*)((u8*)msgctrlEnemyTmons2),
    (void*)0x58000000,
    (void*)((u8*)msgctrlSpeabiNamea),
    (void*)0x58000000,
    (void*)((u8*)msgctrlSpeabiNamed),
    (void*)0x58000000,
    (void*)((u8*)msgctrlSpeabiNamec),
    (void*)0x58000000,
    (void*)((u8*)msgctrlSpeabiNamet),
    (void*)0x58000000,
    (void*)((u8*)msgctrlClientnowork),
    (void*)0x58000000,
    (void*)((u8*)msgctrlSideAttackNameha),
    (void*)0x58000000,
    (void*)((u8*)msgctrlSideAttackNamewo),
    (void*)0x58000000,
    (void*)((u8*)msgctrlSideAttackNameno),
    (void*)0x58000000,
    (void*)((u8*)msgctrlTrainerType),
    (void*)0x58000000,
    (void*)((u8*)msgctrlTrainerName),
    (void*)0x58000000,
    (void*)((u8*)msgctrlTrainerLose),
    (void*)0x58000000,
    (void*)((u8*)msgctrlTrainerEnename),
    (void*)0x58000000,
    (void*)((u8*)msgctrlTrainerEnename2),
    (void*)0x58000000,
    (void*)((u8*)msgctrlTrainerClientno),
    (void*)0x58000000,
    (void*)((u8*)msgctrlWazaName),
    (void*)0x58000000,
    (void*)((u8*)msgctrlItemName),
    (void*)0x58000000,
    (void*)((u8*)msgctrlPasoName),
    (void*)0x58000000,
    (void*)((u8*)msgctrlHero),
    (void*)0x58000000,
    (void*)((u8*)msgctrlHizuki),
    (void*)0x98000000,
    (void*)((u8*)msgctrlItem),
    (void*)0x98000000,
    (void*)((u8*)msgctrlItem2),
    (void*)0x58000000,
    (void*)((u8*)msgctrlDigit),
    (void*)0x58000000,
    (void*)((u8*)msgctrlDigit2),
    (void*)0x98000000,
    (void*)((u8*)msgctrlMsgID),
    (void*)0x58000000,
    (void*)((u8*)msgctrlPokemon),
    (void*)0x58000000,
    (void*)((u8*)msgctrlPokemon2),
    (void*)0x58000000,
    (void*)((u8*)msgctrlMenuDigit),
    (void*)0x58000000,
    (void*)((u8*)msgctrlMenuDigit2),
    (void*)0x58000000,
    (void*)((u8*)msgctrlMenuPokemon),
    (void*)0x58000000,
    (void*)((u8*)msgctrlMenuMsg),
    (void*)0x18000000,
    (void*)((u8*)msgctrlPalette),
    (void*)0x98000000,
    (void*)((u8*)msgctrlWaza),
    (void*)0x98000000,
    (void*)0x00000000,
    (void*)0x98000000,
    (void*)0x00000000,
    (void*)0x58000000,
    (void*)0x00000000,
    (void*)0x10000000,
    (void*)((u8*)msgctrlSeOff),
    (void*)0x10000000,
    (void*)((u8*)msgctrlSeOn),
    (void*)0x18000000,
    (void*)0x00000000,
    (void*)0x18000000,
    (void*)0x00000000,
    (void*)0x58000000,
    (void*)((u8*)msgctrlEvStrBuf2),
    (void*)0x58000000,
    (void*)((u8*)msgctrlSideDefenceNameha),
    (void*)0x58000000,
    (void*)((u8*)msgctrlSideDefenceNamewo),
    (void*)0x58000000,
    (void*)((u8*)msgctrlSideDefenceNameno),
    (void*)0x58000000,
    (void*)((u8*)msgctrlMenuUDigit),
    (void*)0x58000000,
    (void*)((u8*)msgctrlMenuUDigit2),
    (void*)0x58000000,
    (void*)((u8*)msgctrlMenuHex),
    (void*)0x58000000,
    (void*)((u8*)msgctrlMenuHex2),
    (void*)0x58000000,
    (void*)((u8*)msgctrlMenuZDigit),
    (void*)0x58000000,
    (void*)((u8*)msgctrlMenuZDigit2),
    (void*)0x58000000,
    (void*)((u8*)msgctrlMoney),
    (void*)0x58000000,
    (void*)((u8*)msgctrlTime),
    (void*)0x58000000,
    (void*)((u8*)msgctrlString),
    (void*)0x98000000,
    (void*)((u8*)msgctrlPokemonID),
    (void*)0x58000000,
    (void*)((u8*)msgctrlMenuFullDigit),
    (void*)0x58000000,
    (void*)((u8*)msgctrlMenuMoney),
    (void*)0x58000000,
    (void*)((u8*)msgctrlMenuMsg2),
    (void*)0x08000000,
    (void*)((u8*)msgctrlShadow),
    (void*)0x08000000,
    (void*)((u8*)msgctrlAlign),
    (void*)0x10000000,
    (void*)((u8*)msgctrlTalkSE),
    (void*)0x98000000,
    (void*)((u8*)msgctrlMenuMsgID),
    (void*)0x98000000,
    (void*)((u8*)msgctrlMenuMsgID2),
    (void*)0x58000000,
    (void*)((u8*)msgctrlString2),
    (void*)0x98000000,
    (void*)((u8*)msgctrlTribe),
    (void*)0x98000000,
    (void*)((u8*)msgctrlNpc),
    (void*)0x10000000,
    (void*)((u8*)msgctrlIndentOff),
    (void*)0x18000000,
    (void*)((u8*)msgctrlLineSpace),
    (void*)0x18000000,
    (void*)((u8*)msgctrlBaseLineBias),
    (void*)0x10000000,
    (void*)((u8*)msgctrlSndPlay),
    (void*)0x30000000,
    (void*)((u8*)msgctrlSndWait),
};

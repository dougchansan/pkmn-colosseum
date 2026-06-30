# Function-name proposals from log/assert strings

84 candidate(s); 40 HIGH-confidence & currently unnamed.
Advisory only — review before applying. Never written to asm.

| fn | addr | proposed | conf | via | evidence |
|---|---|---|---|---|---|
| `ddh_cc_read` ⟨named⟩ ⟨shared-prefix⟩ | 800C3D9C | **cc_read** | HIGH | leading | `cc_read : error reading bytes from EXI2 %ld\n` |
| `fn_8009E7B0` | 8009E7B0 | **OSLink** | HIGH | leading | `OSLink: unknown relocation type %3d\n` |
| `fn_8009EDAC` | 8009EDAC | **OSUnlink** | HIGH | leading | `OSUnlink: unknown relocation type %3d\n` |
| `fn_800BE800` | 800BE800 | **MessageSend** | HIGH | leading | `MessageSend : cc_write returned %ld\n` |
| `fn_800BF95C` | 800BF95C | **SendACK** | HIGH | leading | `SendACK : Calling MessageSend\n` |
| `fn_800C4D8C` | 800C4D8C | **InitDefaultHeap** | HIGH | file:func | `GCN_Mem_Alloc.c : InitDefaultHeap. No Heap Available\n` |
| `fn_800D39E0` ⟨shared-prefix⟩ | 800D39E0 | **GSgfx** | HIGH | leading | `GSgfx: unable to allocate gsgfx state!\n` |
| `fn_800D7E5C` ⟨shared-prefix⟩ | 800D7E5C | **GSgfx** | HIGH | leading | `GSgfx: matrix stack underflow!\n` |
| `fn_800DF498` | 800DF498 | **GSmaterialSetPEdescr** | HIGH | leading | `GSmaterialSetPEdescr: Warning: already using a custom description!\n` |
| `fn_800DF7A4` | 800DF7A4 | **GSmaterialCreate** | HIGH | leading | `GSmaterialCreate: Run out of materials. Increase materialcount at initialisation` |
| `fn_800DFABC` | 800DFABC | **GSmaterial** | HIGH | leading | `GSmaterial: Unsupported texture format for environment map!\n` |
| `fn_800E2DB0` ⟨shared-prefix⟩ | 800E2DB0 | **GSmem** | HIGH | leading | `GSmem: Warning -- memory loss of %d bytes due to no block before allocation poin` |
| `fn_800E3568` ⟨shared-prefix⟩ | 800E3568 | **GSmem** | HIGH | leading | `GSmem: Init OK, using area %08Xh -> %08Xh\n` |
| `fn_800EE2C8` ⟨shared-prefix⟩ | 800EE2C8 | **GSpart** | HIGH | leading | `GSpart: part is already registered for rotation\n` |
| `fn_800EE3BC` ⟨shared-prefix⟩ | 800EE3BC | **GSpart** | HIGH | leading | `GSpart: child node too deep\n` |
| `fn_800EF5FC` | 800EF5FC | **GStexture** | HIGH | leading | `GStexture: invalid texture format\n` |
| `fn_800F07A8` | 800F07A8 | **GSthreadCreate** | HIGH | leading | `GSthreadCreate. Warning: 'usesFPU==FALE' OK?\n` |
| `fn_800F09D8` | 800F09D8 | **GSthread** | HIGH | leading | `GSthread: Init OK, maximum of %d threads\n` |
| `fn_8010D3C8` | 8010D3C8 | **GScolsys2Draw** | HIGH | leading | `GScolsys2Draw : can't alloc display list memory.\n` |
| `fn_80114948` | 80114948 | **floorReadCameraPreFunc** | HIGH | leading | `floorReadCameraPreFunc: can't alloc %d bytes of memory\n` |
| `fn_80114CA8` | 80114CA8 | **floorReadMapPreFunc** | HIGH | leading | `floorReadMapPreFunc: can't alloc %d bytes of memory\n` |
| `fn_80117514` | 80117514 | **floorUpdateFieldCamera** | HIGH | leading | `floorUpdateFieldCamera: error updating field camera - divide by zero!` |
| `fn_8013111C` | 8013111C | **GSeffect** | HIGH | leading | `GSeffect: Cannot trigger effect - instance uninitialised: effect ID %d.\n` |
| `fn_80137AA4` | 80137AA4 | **tracefxStartEffect** | HIGH | leading | `tracefxStartEffect: Could not start trail effect!\n` |
| `fn_80138CCC` ⟨shared-prefix⟩ | 80138CCC | **leaffxStartEffect** | HIGH | leading | `leaffxStartEffect: Could not start leaf effect - invalid leaf model: groupRes %d` |
| `fn_80139378` | 80139378 | **_leaffxGenerateLeafData** | HIGH | leading | `_leaffxGenerateLeafData: Could not create leaf clone model.\n` |
| `fn_80139D10` ⟨shared-prefix⟩ | 80139D10 | **leaffxStartEffect** | HIGH | leading | `leaffxStartEffect: Could not start lightning effect - not enough memory!\n` |
| `fn_8013B158` | 8013B158 | **filterStart** | HIGH | leading | `filterStart: Could not start filter effect - invalid filter!\n` |
| `fn_8013B5E4` | 8013B5E4 | **surfEffectStart** | HIGH | leading | `surfEffectStart: Could not start wave effect!\n` |
| `fn_8013C718` | 8013C718 | **seaEffectStart** | HIGH | leading | `seaEffectStart: Could not start sea effect!\n` |
| `fn_8013D804` | 8013D804 | **envMapEffectInit** | HIGH | leading | `envMapEffectInit: Could not initialise env map effect!\n` |
| `fn_8013D908` | 8013D908 | **envMapEffectStart** | HIGH | leading | `envMapEffectStart: Could not start env map effect!\n` |
| `fn_8013DDCC` | 8013DDCC | **blurEffectStart** | HIGH | leading | `blurEffectStart: Could not start blur effect!\n` |
| `fn_8013E658` | 8013E658 | **auraEffectStart** | HIGH | leading | `auraEffectStart: Could not start aura effect!\n` |
| `fn_8013F344` | 8013F344 | **distortionEffectStart** | HIGH | leading | `distortionEffectStart: Could not start distortion effect!\n` |
| `fn_8013F97C` | 8013F97C | **_distortionEffectUpdateMatrices** | HIGH | leading | `_distortionEffectUpdateMatrices: Could not project points on screen` |
| `fn_8013FDD0` | 8013FDD0 | **billboardEffectStart** | HIGH | leading | `billboardEffectStart: Could not start billboard effect!\n` |
| `fn_801657F8` | 801657F8 | **soundStop** | HIGH | leading | `soundStop: Warning! BGM cannot be stopped.(snd_id=%d)\n` |
| `fn_80191F64` | 80191F64 | **HSD_ArchiveParse** | HIGH | leading | `HSD_ArchiveParse: byte-order mismatch! Please check data format\n` |
| `fn_801C3D64` | 801C3D64 | **battleGridReplacePokemon** | HIGH | leading | `battleGridReplacePokemon: NULL argument` |
| `fn_801C3F10` | 801C3F10 | **battleGridReplaceTrainer** | HIGH | leading | `battleGridReplaceTrainer: NULL argument!` |
| `fn_801D7E58` | 801D7E58 | **wazaSequenceEntryStop** | HIGH | leading | `wazaSequenceEntryStop: Invalid Data Type!\n` |
| `fn_801D81CC` | 801D81CC | **wazaSequenceEntryUpdate** | HIGH | leading | `wazaSequenceEntryUpdate: Invalid Data Type!\n` |
| `fn_801D84F4` | 801D84F4 | **wazaSequenceEntryStart** | HIGH | leading | `wazaSequenceEntryStart: Invalid Data Type!\n` |
| `fn_801D8B38` | 801D8B38 | **_wazaSequenceParticleEntryStart** | HIGH | leading | `_wazaSequenceParticleEntryStart: Some error occured here\n` |
| `fn_801D9C1C` | 801D9C1C | **wazaSequencePokemonMotionStart** | HIGH | leading | `wazaSequencePokemonMotionStart: Failed to start pokemon motion!\n` |
| `fn_801DB988` | 801DB988 | **wazaSequenceUpdate** | HIGH | leading | `wazaSequenceUpdate: Could not start sequence entry!\n` |
| `fn_801DD45C` | 801DD45C | **sequenceLoad** | HIGH | leading | `sequenceLoad: Error Loading Sequence file!\n` |
| `fn_801DEC64` | 801DEC64 | **_eyeTexAnimEnded** | HIGH | leading | `_eyeTexAnimEnded: Wrong action value!\n` |
| `gdev_cc_read` ⟨named⟩ ⟨shared-prefix⟩ | 800C4348 | **cc_read** | HIGH | leading | `cc_read : error reading bytes from EXI2 %ld\n` |
| `TRKTargetInterrupt` ⟨named⟩ ⟨shared-prefix⟩ | 800C17CC | TargetDoStep | MED | call | `TargetDoStep()\n` |
| `fn_80071398` ⟨shared-prefix⟩ | 80071398 | _menuPop | MED | call | `0 < _CBC.m_nMenuStackDepth &&\"_menuPop():stack under.\"` |
| `fn_800714C8` ⟨shared-prefix⟩ | 800714C8 | _menuPop | MED | call | `0 < _CBC.m_nMenuStackDepth &&\"_menuPop():stack under.\"` |
| `fn_800715BC` | 800715BC | _menuPush | MED | call | `0 &&\"_menuPush(int eMenuID):stack over.\"` |
| `fn_800A4D28` | 800A4D28 | DVDConvertEntrynumToPath | MED | call | `DVDConvertEntrynumToPath(possibly DVDOpen or DVDChangeDir or DVDOpenDir): specif` |
| `fn_800A501C` | 800A501C | DVDOpen | MED | call | `Warning: DVDOpen(): file '%s' was not found under %s.\n` |
| `fn_800A532C` | 800A532C | DVDReadAsync | MED | call | `DVDReadAsync(): specified area is out of the file  ` |
| `fn_800A541C` | 800A541C | DVDRead | MED | call | `DVDRead(): specified area is out of the file  ` |
| `fn_800A5558` | 800A5558 | DVDSeek | MED | call | `DVDSeek(): offset is out of the file  ` |
| `fn_800A56F0` ⟨shared-prefix⟩ | 800A56F0 | DVDChangeDisk | MED | call | `DVDChangeDisk(): FST in the new disc is too big.   ` |
| `fn_800A61E8` ⟨shared-prefix⟩ | 800A61E8 | DVDChangeDisk | MED | call | `DVDChangeDisk(): FST in the new disc is too big.   ` |
| `fn_800C1558` ⟨shared-prefix⟩ | 800C1558 | TargetDoStep | MED | call | `TargetDoStep()\n` |
| `fn_800C1610` ⟨shared-prefix⟩ | 800C1610 | TargetDoStep | MED | call | `TargetDoStep()\n` |
| `fn_800D7D10` ⟨shared-prefix⟩ | 800D7D10 | GSgfx | MED | leading | `GSgfx: invalid matrix index\n` |
| `fn_800D7D90` ⟨shared-prefix⟩ | 800D7D90 | GSgfx | MED | leading | `GSgfx: invalid matrix index\n` |
| `fn_800D7F14` ⟨shared-prefix⟩ | 800D7F14 | GSgfx | MED | leading | `GSgfx: matrix stack overflow!\n` |
| `fn_800D8088` ⟨shared-prefix⟩ | 800D8088 | GSgfx | MED | leading | `GSgfx: matrix stack overflow!\n` |
| `fn_800FE010` | 800FE010 | _msgGetSize | MED | call | `_msgGetSize() failed.\n` |
| `fn_80101244` | 80101244 | loadParticle | MED | call | `loadParticle(): loading...\n` |
| `fn_8011432C` ⟨shared-prefix⟩ | 8011432C | floorReadGFLPreFunc | MED | call | `floorReadGFLPreFunc(): can't alloc %d bytes of memory\n` |
| `fn_8011445C` ⟨shared-prefix⟩ | 8011445C | floorReadParticlePreFunc | MED | call | `floorReadParticlePreFunc(): can't alloc %d bytes of memory\n` |
| `fn_801145C0` ⟨shared-prefix⟩ | 801145C0 | floorReadParticlePreFunc | MED | call | `floorReadParticlePreFunc(): can't alloc %d bytes of memory\n` |
| `fn_80114634` | 80114634 | floorReadWZXPreFunc | MED | call | `floorReadWZXPreFunc(): can't alloc %d bytes of memory\n` |
| `fn_801146A4` | 801146A4 | floorReadPKXPreFunc | MED | call | `floorReadPKXPreFunc(): can't alloc %d bytes of memory\n` |
| `fn_80114760` | 80114760 | floorReadTexPreFunc | MED | call | `floorReadTexPreFunc(): can't alloc %d bytes of memory \n` |
| `fn_8011487C` ⟨shared-prefix⟩ | 8011487C | floorReadGFLPreFunc | MED | call | `floorReadGFLPreFunc(): can't alloc %d bytes of memory\n` |
| `fn_80114AE0` ⟨shared-prefix⟩ | 80114AE0 | floorReadGFLPreFunc | MED | call | `floorReadGFLPreFunc(): can't alloc %d bytes of memory\n` |
| `fn_80114D6C` | 80114D6C | floorReadScriptPreFunc | MED | call | `floorReadScriptPreFunc(): can't alloc %d bytes of memory\n` |
| `fn_80114E78` | 80114E78 | floorReadFontPreFunc | MED | call | `floorReadFontPreFunc(): can't alloc %d bytes of memory\n` |
| `fn_80114F84` | 80114F84 | floorReadMsgPreFunc | MED | call | `floorReadMsgPreFunc(): can't alloc %d bytes of memory\n` |
| `fn_80115024` | 80115024 | floorReadNormalPreFunc | MED | call | `floorReadNormalPreFunc(): can't alloc %d bytes of memory\n` |
| `fn_8013A49C` | 8013A49C | electronStartEffect | MED | leading | `electronStartEffect: Could not start electron effect - invalid model: groupRes %` |
| `fn_801A7CFC` | 801A7CFC | hsdIsDescendantOf | MED | call | `hsdIsDescendantOf(info, &hsdMObj)` |
| `fn_801D91EC` | 801D91EC | _wazaSequenceModelEntryStart | MED | leading | `_wazaSequenceModelEntryStart: Some error occured here\n` |

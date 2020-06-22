/*

	Game offsets, top ones need to be updated every
	update generally, and for big updates other ones
	may need to be changed too.

*/
#pragma once

// RainbowSix.exe ->
#define address_game_manager 0x53a3168
#define address_fov_manager 0x53a3160
#define address_round_manager 0x5392a38
#define address_glow_manager 0x605c388
#define address_profile_manager 0x53903e8
#define address_v_table 0x3a73350

// RainbowSix.exe -> game_manager -> 
#define offset_entity_count 0xD0
#define offset_entity_list 0xC8

// RainbowSix.exe -> game_manager -> entity_list ->
#define offset_entity_index 0x8

//fov manager
#define offset_char_fov 0x38;
#define offset_weapon_fov 0x3C;

UINT64 GameManager = 0x5392a38;
UINT64 MaxEntities = 0xD0;
//aimbot
UINT64 OFFSET_HEAD = 0x670;
UINT64 OFFSET_NECK = 0xF70;
UINT64 OFFSET_RIGHT_HAND = 0x6A0;
UINT64 OFFSET_CHEST = 0xFB0;
UINT64 OFFSET_STOMACH = 0xF90;
UINT64 OFFSET_PELVIS = 0xFD0;
UINT64 OFFSET_FEET = 0x6C0;
UINT64 OFFSET_PROFILE_SKELETON = 0x20;
UINT64 OFFSET_MAX_ENTITYS = 0x1D0;

//cam
UINT64 BASE_OFFSET_RENDER_MAN = 0x52f3508;
UINT64 OFFSET_CAMERA[4] = { 0x68, 0x0, 0x130, 0x420 };
UINT64 OFFSET_VIEWFOVX = 0x800; //updated 3/31
UINT64 OFFSET_VIEWFOVY = 0x814; //updated 3/31
UINT64 OFFSET_VIEWFORWARD = 0x7E0; //updated 3/31
UINT64 OFFSET_VIEWUP = 0x7D0; //updated 3/31
UINT64 OFFSET_VIEWRIGHT = 0x7C0; //updated 3/31
UINT64 OFFSET_VIEWTRANSLATION = 0x7F0; //updated 3/31
UINT64 OFFSET_PROFILE_VIEWANGLE[4]{ 0x0, 0x0, 0x0, 0x0 };
UINT64 OFFSET_ENTITY_REPLICATION = 0xA8;
UINT64 OFFSET_ENTITY_REPLICATION_TEAM = 0x1a8;

uintptr_t pGameManager = mRead<uintptr_t>(base_address + address_game_manager);
uintptr_t pEntityList = mRead<uintptr_t>(pGameManager + offset_entity_list);
uintptr_t pRender = mRead<uintptr_t>(base_address + address_profile_manager);
auto pGameRenderer = mRead<UINT_PTR>(pRender + 0x78);
auto pEngineLink = mRead<UINT_PTR>(pGameRenderer + 0x0);
auto pEngine = mRead<UINT_PTR>(pEngineLink + 0x130);
uintptr_t pCamera = mRead<uintptr_t>(pEngine + 0x420);
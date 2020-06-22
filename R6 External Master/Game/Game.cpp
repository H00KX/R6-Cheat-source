#pragma once 
#include "Game.h"
#include "Offsets.h"
#include "Vectors.h"



HWND TargetHWND = FindWindowA("R6Game", NULL);


Vector2 GetDisplaySize()
{
	RECT rect;
	GetClientRect(TargetHWND, &rect);
	return Vector2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));
}

Vector3 GetViewRight()
{
	if (!pCamera)
		return Vector3();

	return mRead<Vector3>(pCamera + OFFSET_VIEWRIGHT);
}

Vector3 GetViewUp()
{
	if (pCamera)
		return Vector3();

	return mRead<Vector3>(pCamera + OFFSET_VIEWUP);
}

Vector3 GetViewForward()
{
	if (!pCamera)
		return Vector3();

	return mRead<Vector3>(pCamera + OFFSET_VIEWFORWARD);
}

Vector3 GetViewTranslation()
{
	if (!pCamera)
		return Vector3();

	return mRead<Vector3>(pCamera + OFFSET_VIEWTRANSLATION);
}

float GetViewFovX()
{
	if (!pCamera)
		return 0.f;

	return mRead<float>(pCamera + OFFSET_VIEWFOVX);
	std::cout << green << " [>] Fov X: " << white; LoadingAnim(1); std::cout << green << pCamera + OFFSET_VIEWFOVX << "\n";
}

float GetViewFovY()
{
	if (!pCamera)
		return 0.f;

	return mRead<float>(pCamera + OFFSET_VIEWFOVY);
	std::cout << green << " [>] Fov Y: " << white; LoadingAnim(1); std::cout << green << pCamera + OFFSET_VIEWFOVY << "\n";
}

bool WorldToScreen(Vector3 position, Vector2* Screen)
{
	if (!pCamera)
		return false;

	Vector3 temp = position - GetViewTranslation();

	float x = temp.Dot(GetViewRight());
	float y = temp.Dot(GetViewUp());
	float z = temp.Dot(GetViewForward() * -1.f);

	Vector2 DisplaySize = GetDisplaySize();

	Screen->x = (DisplaySize.x / 2.f) * (1.f + x / GetViewFovX() / z);
	Screen->y = (DisplaySize.y / 2.f) * (1.f - y / GetViewFovY() / z);
	std::cout << green << " [>] Display Size (X): " << white; LoadingAnim(1); std::cout << green << DisplaySize.x << "\n";
	std::cout << green << " [>] Display Size (Y): " << white; LoadingAnim(1); std::cout << green << DisplaySize.y << "\n";
	return z >= 1.0f ? true : false;
}


Vector4 CreateFromYawPitchRoll(float yaw, float pitch, float roll)
{
	Vector4 result;
	float cy = cos(yaw * 0.5);
	float sy = sin(yaw * 0.5);
	float cr = cos(roll * 0.5);
	float sr = sin(roll * 0.5);
	float cp = cos(pitch * 0.5);
	float sp = sin(pitch * 0.5);

	result.w = cy * cr * cp + sy * sr * sp;
	result.x = cy * sr * cp - sy * cr * sp;
	result.y = cy * cr * sp + sy * sr * cp;
	result.z = sy * cr * cp - cy * sr * sp;
	return result;
}

void set_angles(uint64_t local_player, Vector3& angles)
{
	float d2r = 0.01745329251f;
	Vector4 new_angles = CreateFromYawPitchRoll(angles.z * d2r, 0.f, angles.x * d2r);

	uint64_t pSkeleton = mRead<uint64_t>(local_player + 0x20);

	if (pSkeleton)
	{
		uint64_t viewAngle2 = mRead<uint64_t>(pSkeleton + 0x11B8);
		mWrite(viewAngle2 + 0xC0, new_angles);
		std::cout << green << " [>] View Angle: " << white; LoadingAnim(1); std::cout << green << viewAngle2 << "\n";
	}
}

Vector3 get_player_head(uint64_t player)
{
	if (!player)
		return Vector3();

	uint64_t pSkeleton = mRead<uint64_t>(player + 0x20);

	if (!pSkeleton)
		return Vector3();

	return mRead<Vector3>(pSkeleton + OFFSET_HEAD);
	std::cout << green << " [>] Skeleton: " << white; LoadingAnim(1); std::cout << green << pSkeleton + OFFSET_HEAD << "\n";
}
Vector3 CalcAngle(Vector3 enemypos, Vector3 camerapos)
{
	float r2d = 57.2957795131f;

	Vector3 dir = enemypos - camerapos;

	float x = asin(dir.z / dir.Length()) * r2d;
	float z = atan(dir.y / dir.x) * r2d;

	if (dir.x >= 0.f) z += 180.f;
	if (x > 180.0f) x -= 360.f;
	else if (x < -180.0f) x += 360.f;

	return Vector3(x, 0.f, z + 90.f);

}

Vector2 DisplaySize = GetDisplaySize();

float flCenterX = (DisplaySize.x / 2.f);
float flCenterY = (DisplaySize.y / 2.f);

bool targetWasIterated = false;
uint64_t local_player = 0;
uint64_t m_target = 0;

unsigned short GetPlayerTeam(DWORD64 Player)
{
	DWORD64 Replication = mRead<DWORD64>(Player + OFFSET_ENTITY_REPLICATION);
	if (!Replication)
		return 0xFF; // Means bot - they dont have entity info structure

	unsigned short OnlineTeamID = mRead<unsigned short>(Replication + OFFSET_ENTITY_REPLICATION_TEAM);
	std::cout << green << " [>] Team ID: " << white; LoadingAnim(1); std::cout << green << OnlineTeamID << "\n";
	return (OnlineTeamID);
}
DWORD64 GetEntityByID(int it, DWORD64 LocalPlayer)
{
	unsigned short LocalTeam = GetPlayerTeam(LocalPlayer);
	if (LocalTeam == 0xFF)
		return NULL;

	DWORD64 Entity = mRead<DWORD>(pEntityList + (sizeof(PVOID) * it));
	if (!Entity)
		return NULL;

	if (LocalPlayer == Entity)
		return NULL;

	unsigned short EnemyTeam = GetPlayerTeam(Entity);
	if (EnemyTeam == LocalTeam)
		return NULL;
	std::cout << green << " [>] Entity Captured: " << white; LoadingAnim(1); std::cout << green << Entity << "\n";
	return Entity;
}
namespace RainbowSix
{
	uintptr_t GetLocalPlayer()
	{
		uintptr_t status_manager = mRead<uintptr_t>(base_address + address_profile_manager);
		if (!status_manager) return NULL;

		uintptr_t entity_container = mRead<uintptr_t>(status_manager + 0x68); if (!entity_container) return NULL;
		entity_container = mRead<uintptr_t>(entity_container + 0x0); if (!entity_container) return NULL;
		return mRead<uintptr_t>(entity_container + 0x28);
	}
	void Esp(bool enable)
	{
		if (enable) {
			uintptr_t game_manager = mRead<uintptr_t>(base_address + address_game_manager);
			if (!game_manager) return;

			uintptr_t entity_list = mRead<uintptr_t>(game_manager + offset_entity_list);
			int entity_count = mRead<DWORD>(game_manager + offset_entity_count) & 0x3fffffff;
			if (!entity_list || !entity_count) return;

			for (int i = 0; i < entity_count; i++)
			{
				uintptr_t entity_object = mRead<uintptr_t>(entity_list + i * 0x8);
				if (!entity_object) continue;

				uintptr_t chain2 = mRead<uintptr_t>(entity_object + 0x28); if (!chain2) continue;
				uintptr_t chain3 = mRead<uintptr_t>(chain2 + 0xD8); if (!chain3) continue;

				for (auto current_component = 0x80; current_component < 0xf0; current_component += sizeof(std::uintptr_t))
				{
					uintptr_t chain4 = mRead<uintptr_t>(chain3 + current_component);
					if (!chain4) continue;

					if (mRead<uintptr_t>(chain4) != (base_address + address_v_table)) continue;

					mWrite<BYTE>(chain4 + 0x552, Options::CavESPStat);
					mWrite<BYTE>(chain4 + 0x554, Options::CavESPStat);
				}
			}
		}
	}
	void GlowEsp(bool enable)
	{
		if (enable) {
			float writeOpacity, writeAlpha, writeDistance;
			{
				if (Options::f_GlowStat) { writeOpacity = 0.f; writeAlpha = 0.f; }
				else { writeOpacity = 100.f; writeAlpha = 1.f; }

				if (Options::f_GlowWeapon) { writeDistance = 0.0f; }
				else { writeDistance = 3.0f; }
			}

			uintptr_t glowbase = mRead<uintptr_t>(base_address + address_glow_manager); if (!glowbase) return;
			uintptr_t glowbase1 = mRead<uintptr_t>(glowbase + 0xB8); if (!glowbase1) return;

			mWrite<float>(glowbase1 + 0xD0, Options::fGlowValue[0]); //red
			mWrite<float>(glowbase1 + 0xD4, Options::fGlowValue[1]); //blue
			mWrite<float>(glowbase1 + 0xD8, Options::fGlowValue[2]); //green

			mWrite<float>(glowbase1 + 0x11C, writeOpacity);

			mWrite<float>(glowbase1 + 0x110, 0); // dist.
			mWrite<float>(glowbase1 + 0x110 + 0x4, 0); // dist.

		}
	}
	void SpecialGlowBlue(bool enable)
	{
		if (enable) {
			uintptr_t glowbase = mRead<uintptr_t>(base_address + address_glow_manager); if (!glowbase) return;
			uintptr_t glowbase1 = mRead<uintptr_t>(glowbase + 0xB8); if (!glowbase1) return;

			mWrite<float>(glowbase1 + 0xD0, 21.f);
			mWrite<float>(glowbase1 + 0xD4, 244.f);
			mWrite<float>(glowbase1 + 0xD8, 238.f);
			mWrite<float>(glowbase1 + 0x11C, 1.7f); // Opacity
			mWrite<float>(glowbase1 + 0x110, 0); // dist.
			mWrite<float>(glowbase1 + 0x110 + 0x4, 0); // dist.
		}
	}
	void SpecialGlowPurple(bool enable)
	{
		if (enable) {
			uintptr_t glowbase = mRead<uintptr_t>(base_address + address_glow_manager); if (!glowbase) return;
			uintptr_t glowbase1 = mRead<uintptr_t>(glowbase + 0xB8); if (!glowbase1) return;

			mWrite<float>(glowbase1 + 0xD0, 99.f);
			mWrite<float>(glowbase1 + 0xD4, 13.f);
			mWrite<float>(glowbase1 + 0xD8, 208.f);
			mWrite<float>(glowbase1 + 0x11C, 2.f);
			mWrite<float>(glowbase1 + 0x110, 0); // dist.
			mWrite<float>(glowbase1 + 0x110 + 0x4, 0); // dist.
		}
	}
	void Recoil()
	{
			BYTE viewRecoil;
			if (Options::f_Recoil < 1.0f) { viewRecoil = 0; }
			else { viewRecoil = 1; }

			uintptr_t lpVisualCompUnk = mRead<uintptr_t>(RainbowSix::GetLocalPlayer() + 0x90); if (!lpVisualCompUnk) return;
			uintptr_t lpWeapon = mRead<uintptr_t>(lpVisualCompUnk + 0xC8); if (!lpWeapon) return;
			uintptr_t lpCurrentDisplayWeapon = mRead<uintptr_t>(lpWeapon + 0x278); if (!lpCurrentDisplayWeapon) return;

			mWrite<BYTE>(lpCurrentDisplayWeapon + 0x168, viewRecoil);
			mWrite<float>(lpCurrentDisplayWeapon + 0x38, Options::f_Recoil);
			mWrite<float>(lpCurrentDisplayWeapon + 0x15c, Options::f_Recoil);
	}
	void NoFlash(bool enable)
	{
		if (enable) {
			DWORD64 Off1 = mRead<DWORD64>(RainbowSix::GetLocalPlayer() + 0x30);
			DWORD64 Flash1 = mRead<DWORD64>(Off1 + 0x30);
			DWORD64 Flash2 = mRead<DWORD64>(Flash1 + 0x28);
			int Value = 0;
			mWrite<UINT8>(Flash2 + 0x40, Value);
		}
	}
	void CharacterFOV(float fov)
	{
		DWORD64 Off0 = mRead<DWORD64>(base_address + address_fov_manager);
		DWORD64 Off1 = mRead<DWORD64>(Off0 + 0x28);
		DWORD64 Off2 = mRead<DWORD64>(Off1 + 0x0);
		mWrite<float>(Off2 + 0x38, Options::f_CharFov);
	}

	void WeaponFOV(float fov)
	{
		DWORD64 Off0 = mRead<DWORD64>(base_address + address_fov_manager);
		DWORD64 Off1 = mRead<DWORD64>(Off0 + 0x28);
		DWORD64 Off2 = mRead<DWORD64>(Off1 + 0x0);
		mWrite<float>(Off2 + 0x3C, Options::f_WeaponFov);
	}
	void Aimbot(bool enable)
	{
		if (enable) {
			unsigned short local_team = GetPlayerTeam(RainbowSix::GetLocalPlayer());

			uintptr_t game_manager = mRead<uintptr_t>(base_address + address_game_manager);
			if (!game_manager) return;

			uintptr_t entity_list = mRead<uintptr_t>(game_manager + offset_entity_list);
			int entity_count = mRead<DWORD>(game_manager + offset_entity_count) & 0x3fffffff;
			if (!entity_list || !entity_count) return;

			for (uint32_t i = 0; i < entity_count; i++)
			{
				uint64_t target_player = GetEntityByID(i, RainbowSix::GetLocalPlayer());

				if (!target_player)
					continue;

				if (target_player == RainbowSix::GetLocalPlayer())
					continue;

				if (GetPlayerTeam(target_player) == local_team)
					continue;

				Vector3 head_position = get_player_head(target_player);

				Vector2 vHeadScreen;
				WorldToScreen(head_position, &vHeadScreen);

				if (vHeadScreen.x - flCenterX < 70.f && flCenterX - vHeadScreen.x < 70.f && flCenterY - vHeadScreen.y < 70.f && vHeadScreen.y - flCenterY < 70.f)
				{
					if (GetAsyncKeyState(VK_RBUTTON) & 0x8000 && m_target != 0 && m_target == target_player)
					{
						Vector3 angles = CalcAngle(head_position, GetViewTranslation());
						angles.clamp();
						set_angles(local_player, angles);
						targetWasIterated = true;
						std::cout << green << " [>] Set Angle X: " << white; LoadingAnim(1); std::cout << green << angles.x << "\n";
						std::cout << green << " [>] Set Angle Y: " << white; LoadingAnim(1); std::cout << green << angles.y << "\n";
						std::cout << green << " [>] Set Angle Z: " << white; LoadingAnim(1); std::cout << green << angles.z << "\n";
					}
					else if (GetAsyncKeyState(VK_RBUTTON) && m_target == 0)
					{
						m_target = target_player;
						std::cout << green << " [>] Target Player: " << white; LoadingAnim(1); std::cout << green << target_player << "\n";
						targetWasIterated = true;
					}

					else if (!GetAsyncKeyState(VK_RBUTTON))
						m_target = 0;
				}
			}
		}
	}
}

// For outline
BYTE GetRoundState()
{
	uintptr_t round_manager = mRead<uintptr_t>(base_address + address_round_manager);
	if (!round_manager) return 0;

	return mRead<BYTE>(round_manager + 0x2E8);
}
void SetColour(int colour)
{
	uintptr_t game_manager = mRead<uintptr_t>(base_address + address_game_manager);
	if (!game_manager) return;

	uintptr_t chain = mRead<uintptr_t>(game_manager + 0x320); if (!chain) return;
	chain = mRead<uintptr_t>(chain + 0x968); if (!chain) return;

	mWrite<float>(chain + 0x54, 1.f); // Alpha

	if (colour == 0) // Wants to set enemy colour
	{
		mWrite<float>(chain + 0x48, 1.f);
		mWrite<float>(chain + 0x4C, 0.f);
		mWrite<float>(chain + 0x50, 0.f);
	}
	else if (colour == 1)
	{
		mWrite<float>(chain + 0x48, 0.f);
		mWrite<float>(chain + 0x4C, 1.f);
		mWrite<float>(chain + 0x50, 0.f);
	}
	return;
}
bool IsScanning()
{
	uintptr_t game_manager = mRead<uintptr_t>(base_address + address_game_manager);
	if (!game_manager) return false;

	uintptr_t chain = mRead<uintptr_t>(game_manager + 0x320); if (!chain) return false;
	chain = mRead<uintptr_t>(chain + 0x968); if (!chain) return false;

	return mRead<BYTE>(chain + 0x310);
}
BYTE GetArray()
{
	uintptr_t game_manager = mRead<uintptr_t>(base_address + address_game_manager);
	if (!game_manager) return 0;

	uintptr_t chain = mRead<uintptr_t>(game_manager + 0x320); if (!chain) return 0;
	chain = mRead<uintptr_t>(chain + 0x968); if (!chain) return 0;

	return mRead<BYTE>(chain + 0x2F8);
}
void WriteArray(BYTE writethis)
{
	uintptr_t game_manager = mRead<uintptr_t>(base_address + address_game_manager);
	if (!game_manager) return;

	uintptr_t chain = mRead<uintptr_t>(game_manager + 0x320); if (!chain) return;
	chain = mRead<uintptr_t>(chain + 0x968); if (!chain) return;

	mWrite<BYTE>(chain + 0x2F8, writethis);
	return;
}
void SwapTeams()
{
	uintptr_t profile_manager = mRead<uintptr_t>(base_address + address_profile_manager);
	if (!profile_manager) return;

	uintptr_t chain = mRead<uintptr_t>(profile_manager + 0x68); if (!chain) return;
	chain = mRead<uintptr_t>(chain + 0x0); if (!chain) return;
	chain = mRead<uintptr_t>(chain + 0x28); if (!chain) return;
	chain = mRead<uintptr_t>(chain + 0xc8); if (!chain) return;
	chain = mRead<uintptr_t>(chain + 0x88); if (!chain) return;
	BYTE original_team = mRead<BYTE>(chain + 0x30);

	if (original_team == 3)
	{
		mWrite<BYTE>(chain + 0x30, 4);
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		mWrite<BYTE>(chain + 0x30, 3);
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	else if (original_team == 4)
	{
		mWrite<BYTE>(chain + 0x30, 3);
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		mWrite<BYTE>(chain + 0x30, 4);
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	return;
}
void SetOutline()
{
	BYTE toggle = 1;
	int toggle2 = 0;
	BYTE toggle3 = 4;
	BYTE ArraySize = 5;

	SetColour(1);
	SwapTeams();
	uintptr_t game_manager = mRead<uintptr_t>(base_address + address_game_manager);
	if (!game_manager) return;
	// Increase array size
	DWORD64 GMComponentContainer = mRead<DWORD64>(game_manager + 0x320);
	DWORD64 GMComponentMarkAndOutline = mRead<DWORD64>(GMComponentContainer + 0x968);
	mWrite<BYTE>(GMComponentMarkAndOutline + 0x2F8, ArraySize);

	uintptr_t entity_list = mRead<uintptr_t>(game_manager + offset_entity_list);
	int entity_count = mRead<DWORD>(game_manager + offset_entity_count) & 0x3fffffff;
	if (!entity_list || !entity_count) return;

	for (int i = 0; i < entity_count; i++)
	{
		DWORD64 tmp = mRead<DWORD64>(game_manager + 0x1d8);
		DWORD64 tmp1 = mRead<DWORD64>(tmp + i * 0x8);
		DWORD64 tmp2 = mRead<DWORD64>(tmp1 + 0xA70);
		DWORD64 tmp3 = mRead<DWORD64>(tmp2 + 0x30);
		DWORD64 tmp4 = mRead<DWORD64>(tmp3 + 0x48);
		mWrite<BYTE>(tmp4 + 0x110, toggle);
	}

	// Freeze array
	mWrite<BYTE>(GMComponentMarkAndOutline + 0x310, toggle2);

	return;
}



